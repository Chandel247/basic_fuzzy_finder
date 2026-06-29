#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

void dir_parser(std::vector<fs::path> &files, std::string curr) {
  const std::vector<std::string> exDir = {"build", ".git", ".venv"};
  fs::path base_dir{curr};

  try {
    auto start = fs::recursive_directory_iterator(curr);
    auto end = fs::recursive_directory_iterator();

    while (start != end) {
      if (start->is_regular_file()) {
        fs::path target_file{start->path()};
        fs::path rel_path = fs::relative(target_file, base_dir);
        files.push_back(rel_path);
      }
      if (start->is_directory()) {
        std::string dirName = start->path().filename().string();
        if (std::find(exDir.begin(), exDir.end(), dirName) != exDir.end()) {
          start.disable_recursion_pending();
        } else {
          fs::path target_file{start->path()};
          fs::path rel_path = fs::relative(target_file, base_dir);
          files.push_back(rel_path);
        }
      }
      start++;
    }
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Error: " << e.what() << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }
}

double fuzzy_score(const fs::path &file_obj, const std::string &query) {
  double score = 0;
  int last_match_pos = -2;
  int query_pointer = 0;
  int file_name_pointer = 0;
  int level_counter;
  std::string file_name = file_obj.filename().string();
  std::string path_name = file_obj.string();
  if (query.length() == 0)
    return 0;
  level_counter = std::count(path_name.begin(), path_name.end(), '/');
  score -= (level_counter - 1);
  while (file_name_pointer != (int)file_name.length()) {
    char small_query = std::tolower(query[query_pointer]);
    char small_file_name = std::tolower(file_name[file_name_pointer]);
    if (small_query == small_file_name) {
      score++;
      if ((file_name_pointer - last_match_pos) == 1) {
        score += 0.6;
      }
      if (query[query_pointer] == file_name[file_name_pointer]) {
        score += 0.8;
      }
      last_match_pos = file_name_pointer;
      query_pointer++;
      if (query_pointer == (int)query.length()) {
        break;
      }
    }
    file_name_pointer++;
  }
  if (query_pointer < (int)query.length()) {
    return 0;
  }
  return score;
}

void enable_raw_mode(termios &orig) {
  tcgetattr(STDIN_FILENO, &orig);
  termios raw = orig;
  raw.c_lflag &= ~(ICANON | ECHO);
  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(termios &orig) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
}

// After reading \033, try to read the next 2 bytes with a 100 ms timeout.
// Returns true and fills seq[2] if an arrow key sequence ([A or [B) is found.
bool read_escape_seq(char seq[2]) {
  termios t;
  tcgetattr(STDIN_FILENO, &t);
  termios timeout_t = t;
  timeout_t.c_cc[VMIN] = 0;
  timeout_t.c_cc[VTIME] = 1;
  tcsetattr(STDIN_FILENO, TCSANOW, &timeout_t);

  bool valid = false;
  if (read(STDIN_FILENO, &seq[0], 1) == 1 && seq[0] == '[') {
    if (read(STDIN_FILENO, &seq[1], 1) == 1) {
      valid = (seq[1] == 'A' || seq[1] == 'B');
    }
  }

  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
  return valid;
}

std::vector<fs::path> top_matches(const std::vector<fs::path> &files,
                                   const std::string &query, int limit = 10) {
  if (query.empty()) {
    int count = std::min((int)files.size(), limit);
    return std::vector<fs::path>(files.begin(), files.begin() + count);
  }

  std::vector<std::pair<double, fs::path>> scored;
  for (const auto &f : files) {
    double s = fuzzy_score(f, query);
    if (s > 0)
      scored.push_back({s, f});
  }
  std::sort(scored.begin(), scored.end(),
            [](const auto &a, const auto &b) { return a.first > b.first; });

  std::vector<fs::path> result;
  int count = std::min((int)scored.size(), limit);
  for (int i = 0; i < count; i++)
    result.push_back(scored[i].second);
  return result;
}

void redraw(const std::string &query, const std::vector<fs::path> &results,
            int cursor) {
  int total_lines = 1 + (int)results.size();

  std::cerr << "\r\033[J";
  std::cerr << "> " << query << "\n";

  for (int i = 0; i < (int)results.size(); i++) {
    if (i == cursor)
      std::cerr << "\033[7m\xe2\x86\x92 " << results[i].string() << "\033[0m\n";
    else
      std::cerr << "  " << results[i].string() << "\n";
  }

  // Move back up so the next redraw overwrites the same area
  std::cerr << "\033[" << total_lines << "A";
  std::cerr.flush();
}

int main() {
  std::vector<fs::path> files;
  dir_parser(files, fs::current_path().string());

  termios orig;
  enable_raw_mode(orig);

  std::string query;
  int cursor = 0;
  std::vector<fs::path> results = top_matches(files, query);
  redraw(query, results, cursor);

  char c;
  while (read(STDIN_FILENO, &c, 1) == 1) {
    if (c == 3) {  // Ctrl-C
      disable_raw_mode(orig);
      std::cerr << "\r\033[J";
      return 1;
    } else if (c == 27) {  // Escape or arrow key
      char seq[2];
      if (read_escape_seq(seq)) {
        if (seq[1] == 'A') {  // up arrow
          if (cursor > 0)
            cursor--;
        } else if (seq[1] == 'B') {  // down arrow
          if (cursor < (int)results.size() - 1)
            cursor++;
        }
      } else {
        disable_raw_mode(orig);
        std::cerr << "\r\033[J";
        return 1;
      }
    } else if (c == '\r' || c == '\n') {  // Enter
      disable_raw_mode(orig);
      std::cerr << "\r\033[J";
      if (!results.empty())
        std::cout << results[cursor].string() << "\n";
      return 0;
    } else if (c == 127 || c == 8) {  // Backspace
      if (!query.empty())
        query.pop_back();
      cursor = 0;
    } else if (isprint(c)) {
      query += c;
      cursor = 0;
    }
    results = top_matches(files, query);
    cursor = std::min(cursor, std::max(0, (int)results.size() - 1));
    redraw(query, results, cursor);
  }

  disable_raw_mode(orig);
  std::cerr << "\r\033[J";
  return 0;
}
