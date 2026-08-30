cmake_minimum_required(VERSION 3.14)
project(BASIC_FF LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(fuzzy_finder
    main.cpp
    tui.cpp
    parsers/dir_parsers.cpp
    scorers/fuzzy_scorers.cpp
)

target_include_directories(fuzzy_finder PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
