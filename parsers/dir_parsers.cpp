#include "dir_parsers.hpp"
#include <iostream>
#include <algorithm>

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
