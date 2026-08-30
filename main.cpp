#include "parsers/dir_parsers.hpp"
#include "tui.hpp"
#include <iostream>

namespace fs = std::filesystem;

int main() {
  std::vector<fs::path> files_dir_paths;
  dir_parser(files_dir_paths, fs::current_path().string());

  auto selected = run_tui(files_dir_paths);
  if (!selected)
    return 1;

  std::cout << selected->string() << "\n";
  return 0;
}
