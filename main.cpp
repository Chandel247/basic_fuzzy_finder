#include "parsers/dir_parsers.hpp"
#include "scorers/fuzzy_scorers.hpp"
#include <iostream>

namespace fs=std::filesystem;
int main() {
  std::vector<fs::path> files_dir_paths;
  dir_parser(files_dir_paths, "/Users/shivpratapsinghchandel/Random");
  for (auto &i:files_dir_paths){
      std::cout<<i<<":"<<fuzzy_score(i, "line")<<"\n";
  }
  return 0;
}
