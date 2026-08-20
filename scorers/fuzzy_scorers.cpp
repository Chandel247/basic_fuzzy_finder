#include "fuzzy_scorers.hpp"
namespace fs=std::filesystem;

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
