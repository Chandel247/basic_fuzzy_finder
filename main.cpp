#include <algorithm>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
namespace fs=std::filesystem;

void dir_parser(std::vector<std::string> &files, std::string curr){
    const std::vector<std::string> exDir={"build", ".git", ".venv"};
 
    auto start=fs::recursive_directory_iterator(curr);
    auto end=fs::recursive_directory_iterator();

    while (start!=end){
        if (start->is_regular_file()){
            files.push_back(start->path().string());
        }
        if (start->is_directory()){
            std::string dirName=start->path().filename().string();
            if (std::find(exDir.begin(),exDir.end(),dirName)!=exDir.end()){
                start.disable_recursion_pending();
                start++;
                continue;
            }
            else {
                files.push_back(start->path().string());
            }
        }
        start++;
    }
}

int main(){

    std::string curr;
    std::getline(std::cin,curr);
    std::vector<std::string> file_names;
    dir_parser(file_names,curr);
    for (std::string file:file_names){
        std::cout<<file<<std::endl;
    }
    return 0;
}
