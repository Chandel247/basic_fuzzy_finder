#pragma once
#ifndef TUI_HPP
#define TUI_HPP

#include <filesystem>
#include <optional>
#include <vector>

// Runs the interactive fuzzy-finder prompt over `files` and returns the
// selected entry, or std::nullopt if the user cancelled (Ctrl-C / Escape).
std::optional<std::filesystem::path>
run_tui(const std::vector<std::filesystem::path> &files);

#endif
