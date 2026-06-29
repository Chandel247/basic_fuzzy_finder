#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>
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
  std::string file_name=file_obj.filename().string();
  std::string path_name=file_obj.string();
  if (query.length() == 0)
    return 0;
  level_counter = std::count(path_name.begin(),path_name.end(), '/');
  score -= (level_counter - 1);
  while (file_name_pointer != file_name.length()) {
    //Ensuring characters are in same case
    char small_query = std::tolower(query[query_pointer]);
    char small_file_name = std::tolower(file_name[file_name_pointer]);
    if (small_query == small_file_name) {
      score++;
      //consecutive bonus
      if ((file_name_pointer - last_match_pos) == 1) {
        score+=0.6;
      }
      //same case bonus
      if (query[query_pointer] == file_name[file_name_pointer]) {
        score+=0.8;
      }
      //updating lass position for consecutive bonus
      last_match_pos = file_name_pointer;
      query_pointer++;
      if (query_pointer == query.length()) {
        break;
      }
    }
    file_name_pointer++;
  }
  if (query_pointer < query.length()) {
    return 0;
  }
  return score;
}

int main() {
  std::string curr="/Users/shivpratapsinghchandel/Random";
  std::getline(std::cin, curr);
  std::vector<fs::path> file_names;
  dir_parser(file_names, curr);
  std::string query="line";
  std::getline(std::cin, query);
  return 0;
}
