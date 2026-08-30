
# Basic Fuzzy Finder

A lightweight fuzzy file finder written in C++ as a learning project to better understand how fuzzy search tools work internally.

## Overview

This project was built primarily as a learning exercise to explore the implementation details behind popular fuzzy finders such as **fzf** and **fzy**. Rather than relying on existing libraries, the goal is to understand the algorithms, scoring mechanisms, and design choices that make these tools fast and effective.

At the same time, I wanted to build a project that was both technically challenging and genuinely interesting to me. I've always enjoyed working with fuzzy finders, and this project gave me an opportunity to explore the concepts behind a tool that I use regularly.

## Current Implementation

The current implementation performs fuzzy matching using the **filename** as the primary search target. To improve the relevance of results, the **full file path** is also considered as a penalty factor, causing files nested deeper within the directory tree to rank lower than equally relevant files located closer to the project root.

This approach provides a simple yet effective scoring mechanism while keeping the implementation easy to understand.

## Future Improvements

This project is still a work in progress, and there are several areas I plan to explore, including:

* Implementing and benchmarking alternative fuzzy matching algorithms, such as **Levenshtein Distance**.
* Experimenting with different scoring and ranking strategies to improve search quality.
* Optimizing the matching algorithm for larger directory trees.
* Adding comprehensive testing and benchmarking.
* Improving the user interface and overall usability.

The primary objective of this project is not to compete with mature fuzzy finders, but to gain a deeper understanding of the algorithms and implementation techniques that make them so efficient.

