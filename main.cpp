#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
namespace fs=std::filesystem;

void dir_parser(std::vector<std::string> &files, std::string curr){
    const std::vector<std::string> exDir={"build", ".git", ".venv"};
    fs::path base_dir{curr};
 
    auto start=fs::recursive_directory_iterator(curr);
    auto end=fs::recursive_directory_iterator();

    while (start!=end){
        if (start->is_regular_file()){
            fs::path target_file{start->path()};
            fs::path rel_path=fs::relative(target_file,base_dir);
            files.push_back(rel_path.string());
        }
        if (start->is_directory()){
            std::string dirName=start->path().filename().string();
            if (std::find(exDir.begin(),exDir.end(),dirName)!=exDir.end()){
                start.disable_recursion_pending();
                start++;
                continue;
            }
            else {
                fs::path target_file{start->path()};
                fs::path rel_path=fs::relative(target_file,base_dir);
                files.push_back(rel_path.string());
            }
        }
        start++;
    }
}

int fuzzy_score(const std::string &file_name, const std::string &query){
    int score=0;
    int last_match_pos=-1;
    int query_pointer=0;
    int file_name_pointer=0;
    while (file_name_pointer!=(file_name.length())){
        if (query[query_pointer]==file_name[file_name_pointer]){
            if (file_name_pointer-last_match_pos==1){
                score+=2;
                last_match_pos=file_name_pointer;
            }
            else {
                score++;
                last_match_pos=file_name_pointer;
            }
            if (query_pointer==query.length()-1){
                break;
            }
            query_pointer++;
        }
        file_name_pointer++;
    }
    return score;
}


int main(){
    std::string curr;
    std::getline(std::cin,curr);
    std::vector<std::string> file_names;
    dir_parser(file_names,curr);
    std::string query;
    std::getline(std::cin,query);
    return 0;
}
