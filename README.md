# basic_fuzzy_finder

A minimal fuzzy finder built from scratch in C++17.

---

## Introduction

`basic_fuzzy_finder` is a command-line tool that recursively walks a directory tree and prints every file and subdirectory path to stdout. The output is designed to be piped into a fuzzy-search tool like `fzf` for fast, interactive file navigation — similar to how editors like VS Code or Neovim implement their "Go to File" feature under the hood.

---

## Why This Project Exists

This project was built as a **learning exercise** to understand how fuzzy finders work at a fundamental level:

- How do they collect the list of files so quickly?
- What directories should be skipped (build artifacts, version control, virtual envs)?
- How does the pipeline between a file-lister and a fuzzy-matcher fit together?

By building the file-listing layer from scratch, the goal was to gain a concrete understanding of the mechanics behind tools like `fzf`, `telescope.nvim`, and similar utilities.

---

## Timeline

| Date | Commit | Milestone |
|------|--------|-----------|
| 2026-06-04 | [`a559907`](../../commit/a559907) | **Milestone 1 — Directory Walker**: Recursive filesystem traversal using `std::filesystem`, filtering noise directories (`build`, `.git`, `.venv`). Outputs all paths to stdout for pipeline use. |
| 2026-06-04 | [`1ab4804`](../../commit/1ab4804) | **Docs**: README restructured with sections and contributions table. |
| In progress | — | **Milestone 2 — Fuzzy Scoring**: `fuzzy_score()` implemented — subsequence matching that rewards consecutive character runs. Wiring into `main()` pending. |

---

## What Has Been Done

- [x] Recursive directory walker in C++17 using `std::filesystem`
- [x] Skips noise directories: `build`, `.git`, `.venv`
- [x] Prints all file and directory paths to stdout for pipeline use
- [x] Pre-compiled binary (`fuzzy_finder.exe`)
- [x] `.gitignore` to exclude binaries and build artifacts
- [x] Repository pushed to GitHub
- [ ] Fuzzy scoring algorithm (`fuzzy_score`) — implemented, not yet wired into `main()`
- [ ] Sort results by score and display ranked output

---

## Build

Requires a C++17-capable compiler (`g++` or `clang++`).

```bash
g++ -std=c++17 -o fuzzy_finder main.cpp
```

---

## Usage

```bash
# List all files under a path
echo "/path/to/search" | ./fuzzy_finder

# Pipe into fzf for interactive fuzzy search
echo "/path/to/search" | ./fuzzy_finder | fzf
```

---

## Contributions

| Contribution | Shiv Pratap Singh Chandel | Claude Code (AI) |
|---|:---:|:---:|
| `main.cpp` — recursive directory walker | ✅ | |
| Core logic: filesystem traversal & directory skipping | ✅ | |
| Compiled binary (`fuzzy_finder.exe`) | ✅ | |
| `.gitignore` — excludes binaries, build dirs, IDE files | | ✅ |
| `README.md` — project documentation | | ✅ |
| Git repository setup & initial commit | | ✅ |
| Remote origin config & push to GitHub | | ✅ |

---

## Author

**Shiv Pratap Singh Chandel**
