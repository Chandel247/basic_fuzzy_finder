#pragma once
#ifndef DIR_PARSERS_HPP
#define DIR_PARSERS_HPP

#include <filesystem>
#include <string>
#include <vector>


void dir_parser(
    std::vector<std::filesystem::path>& files,
    std::string curr = std::filesystem::current_path().string()
);

#endif
