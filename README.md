

# Basic Fuzzy Finder

A lightweight fuzzy file finder written in C++ as a learning project to better understand how fuzzy search tools work internally.

https://github.com/user-attachments/assets/a449a3dc-7581-4fb6-b3f0-99c616252b96

## Overview

This project was built primarily as a learning exercise to explore the implementation details behind popular fuzzy finders such as **fzf** and **fzy**. Rather than relying on existing libraries, the goal is to understand the algorithms, scoring mechanisms, and design choices that make these tools fast and effective.

At the same time, I wanted to build a project that was both technically challenging and genuinely interesting to me. I've always enjoyed working with fuzzy finders, and this project gave me an opportunity to explore the concepts behind a tool that I use regularly.

## Current Implementation

The current implementation performs fuzzy matching using the **filename** as the primary search target. To improve the relevance of results, the **full file path** is also considered as a penalty factor, causing files nested deeper within the directory tree to rank lower than equally relevant files located closer to the project root.

This approach provides a simple yet effective scoring mechanism while keeping the implementation easy to understand.

## Usage

Build the binary and run it from any project directory:

```bash
g++ -std=c++17 -o fuzzy_finder main.cpp
./fuzzy_finder
```

The tool scans the current working directory on startup and presents an interactive search prompt:

| Key | Action |
|-----|--------|
| Any character | Append to query; results update instantly |
| `Backspace` | Remove last character from query |
| `↑` / `↓` | Move selection cursor up/down through results |
| `Enter` | Print selected file path to stdout and exit |
| `Escape` / `Ctrl-C` | Exit without output |

Because the display (query line + result list) is written to **stderr**, the selected path on stdout is pipe-friendly:

```bash
./fuzzy_finder | xargs open
```

## Future Improvements

This project is still a work in progress, and there are several areas I plan to explore, including:

* Implementing and benchmarking alternative fuzzy matching algorithms, such as **Levenshtein Distance**.
* Experimenting with different scoring and ranking strategies to improve search quality.
* Optimizing the matching algorithm for larger directory trees.
* Adding comprehensive testing and benchmarking.
* Improving the terminal UI (multi-column layout, scrolling for large result sets).

The primary objective of this project is not to compete with mature fuzzy finders, but to gain a deeper understanding of the algorithms and implementation techniques that make them so efficient.

