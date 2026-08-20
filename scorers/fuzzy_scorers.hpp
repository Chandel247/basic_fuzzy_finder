#pragma once

#ifndef FUZZY_SCORERS_HPP
#define FUZZY_SCORERS_HPP

#include <filesystem>
#include <string>

double fuzzy_score(const std::filesystem::path &file_obj, const std::string &query);

#endif
