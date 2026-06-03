# basic_ff — Basic Fuzzy Finder

A minimal command-line fuzzy finder written in C++17 that recursively lists all files and directories under a given path.

## What has been done

### `main.cpp`
The core C++ program written by **Shiv Pratap Singh Chandel**. It:

- Accepts a directory path from stdin.
- Recursively walks that directory using `std::filesystem::recursive_directory_iterator`.
- Skips the directories `build`, `.git`, and `.venv` to avoid noise.
- Prints every file and subdirectory path to stdout, one per line.

This output is intended to feed into a fuzzy-search pipeline (e.g. piped to `fzf` or a similar tool) for fast file navigation.

### `fuzzy_finder.exe`
Pre-compiled binary of `main.cpp`, built for the current platform.

### `.gitignore` — added by Claude Code (2026-06-04)
Excludes compiled binaries (`*.exe`, `*.out`, `*.o`, etc.), build directories, CMake artifacts, IDE files (`.vscode/`, `.idea/`), macOS/Windows system files, and dependency directories.

### `README.md` — added by Claude Code (2026-06-04)
This file.

## Build

Requires a C++17-capable compiler (g++ or clang++).

```bash
g++ -std=c++17 -o fuzzy_finder main.cpp
```

## Usage

```bash
echo "/path/to/search" | ./fuzzy_finder
# pipe into fzf for interactive fuzzy search
echo "/path/to/search" | ./fuzzy_finder | fzf
```

## Author

Shiv Pratap Singh Chandel
