#include "tui.hpp"
#include "scorers/fuzzy_scorers.hpp"

#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <string>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace fs = std::filesystem;

namespace {

constexpr int kMaxVisibleRows = 10;
constexpr char kHideCursor[] = "\033[?25l";
constexpr char kShowCursor[] = "\033[?25h";

std::vector<fs::path> top_matches(const std::vector<fs::path> &files,
                                   const std::string &query,
                                   int limit = kMaxVisibleRows) {
  std::vector<std::pair<double, fs::path>> scored;
  scored.reserve(files.size());
  for (const auto &f : files) {
    double s = query.empty() ? 0.0 : fuzzy_score(f, query);
    if (query.empty() || s > 0)
      scored.emplace_back(s, f);
  }
  std::stable_sort(scored.begin(), scored.end(),
                    [](const auto &a, const auto &b) { return a.first > b.first; });

  std::vector<fs::path> result;
  int count = std::min((int)scored.size(), limit);
  result.reserve(count);
  for (int i = 0; i < count; i++)
    result.push_back(scored[i].second);
  return result;
}

int terminal_width() {
  struct winsize ws {};
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
    return ws.ws_col;
  return 80;
}

std::string truncate(const std::string &s, size_t width) {
  if (width == 0)
    return "";
  return s.size() <= width ? s : s.substr(0, width);
}

// Puts the terminal into raw mode and hides the cursor for its lifetime,
// restoring both on destruction -- including on early return, an
// exception unwinding out of run_tui, or a signal that we catch below.
// Only one instance is ever live (run_tui owns it), so a single static
// pointer is enough for the signal handler to reach it.
class RawTerminal {
public:
  RawTerminal() {
    tcgetattr(STDIN_FILENO, &original_);
    termios raw = original_;
    cfmakeraw(&raw);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    write(STDOUT_FILENO, kHideCursor, sizeof(kHideCursor) - 1);
    active_instance = this;
  }

  ~RawTerminal() {
    restore();
    active_instance = nullptr;
  }

  RawTerminal(const RawTerminal &) = delete;
  RawTerminal &operator=(const RawTerminal &) = delete;

  // Async-signal-safe-ish best-effort restore, used both by the destructor
  // and by the signal handler when we're killed from outside the process
  // (so ISIG being off in raw mode doesn't leave the terminal broken).
  void restore() {
    write(STDOUT_FILENO, kShowCursor, sizeof(kShowCursor) - 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &original_);
  }

  static RawTerminal *active_instance;

private:
  termios original_{};
};

RawTerminal *RawTerminal::active_instance = nullptr;

void handle_terminating_signal(int sig) {
  if (RawTerminal::active_instance)
    RawTerminal::active_instance->restore();
  std::signal(sig, SIG_DFL);
  std::raise(sig);
}

void install_signal_handlers() {
  std::signal(SIGINT, handle_terminating_signal);
  std::signal(SIGTERM, handle_terminating_signal);
  std::signal(SIGHUP, handle_terminating_signal);
}

enum class Key { Char, Enter, Backspace, Up, Down, Cancel, None };

struct KeyEvent {
  Key key;
  char ch = 0;
};

// After reading ESC, try to read the next 2 bytes with a short timeout to
// tell an arrow-key sequence (ESC [ A/B) apart from a lone Escape press.
KeyEvent read_escape_seq() {
  termios current;
  tcgetattr(STDIN_FILENO, &current);
  termios probe = current;
  probe.c_cc[VMIN] = 0;
  probe.c_cc[VTIME] = 1; // 100ms
  tcsetattr(STDIN_FILENO, TCSANOW, &probe);

  char seq[2] = {0, 0};
  ssize_t n1 = read(STDIN_FILENO, &seq[0], 1);
  ssize_t n2 = (n1 == 1) ? read(STDIN_FILENO, &seq[1], 1) : 0;

  tcsetattr(STDIN_FILENO, TCSANOW, &current);

  if (n1 == 1 && seq[0] == '[' && n2 == 1) {
    if (seq[1] == 'A')
      return {Key::Up};
    if (seq[1] == 'B')
      return {Key::Down};
  }
  return {Key::Cancel};
}

KeyEvent read_key() {
  char c;
  if (read(STDIN_FILENO, &c, 1) != 1)
    return {Key::Cancel}; // EOF / closed stdin

  if (c == '\r' || c == '\n')
    return {Key::Enter};
  if (c == 127 || c == 8)
    return {Key::Backspace};
  if (c == 3) // Ctrl-C
    return {Key::Cancel};
  if (c == 27)
    return read_escape_seq();
  if (isprint((unsigned char)c))
    return {Key::Char, c};
  return {Key::None};
}

// The cursor is always parked at the start of the prompt line before a
// draw, so redrawing only ever needs a carriage return, never an upward
// cursor move on entry (the loop below always finishes back on that line).
void redraw(const std::string &query, const std::vector<fs::path> &results,
            int selected) {
  int width = terminal_width();
  std::string out = "\r> " + query + "\x1b[K";

  for (int i = 0; i < (int)results.size(); i++) {
    out += "\r\n";
    std::string line =
        truncate(results[i].string(), width > 2 ? (size_t)width - 2 : (size_t)width);
    if (i == selected)
      out += "\x1b[7m" + line + "\x1b[0m";
    else
      out += line;
    out += "\x1b[K";
  }
  out += "\x1b[J"; // wipe any leftover rows from a previous, longer draw

  if (!results.empty())
    out += "\r\x1b[" + std::to_string(results.size()) + "A";
  else
    out += "\r";
  out += "\x1b[" + std::to_string(2 + query.size()) + "C";

  write(STDOUT_FILENO, out.data(), out.size());
}

} // namespace

std::optional<fs::path> run_tui(const std::vector<fs::path> &files) {
  install_signal_handlers();
  RawTerminal raw_terminal;

  std::string query;
  int selected = 0;
  std::vector<fs::path> results = top_matches(files, query);
  std::optional<fs::path> selection;

  redraw(query, results, selected);

  bool running = true;
  while (running) {
    KeyEvent event = read_key();
    switch (event.key) {
    case Key::Char:
      query += event.ch;
      selected = 0;
      break;
    case Key::Backspace:
      if (!query.empty())
        query.pop_back();
      selected = 0;
      break;
    case Key::Up:
      if (selected > 0)
        selected--;
      break;
    case Key::Down:
      if (selected < (int)results.size() - 1)
        selected++;
      break;
    case Key::Enter:
      if (!results.empty())
        selection = results[selected];
      running = false;
      break;
    case Key::Cancel:
      running = false;
      break;
    default:
      break;
    }

    if (running) {
      results = top_matches(files, query);
      selected = std::min(selected, std::max(0, (int)results.size() - 1));
      redraw(query, results, selected);
    }
  }

  // Wipe the whole prompt+list block so the terminal is left clean for
  // whatever the caller prints next. RawTerminal's destructor (about to
  // run) takes care of showing the cursor again and restoring the
  // original termios settings.
  write(STDOUT_FILENO, "\r\x1b[J", 4);

  return selection;
}
