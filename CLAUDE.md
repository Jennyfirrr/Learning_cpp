# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Personal C++ learning repository — coursework (hw/, lab/), LeetCode/HackerRank solutions, terminal graphics experiments, and deep dives into low-level topics (bitwise ops, branchless programming, fixed-point math).

## Build Commands

No build system (no Makefile/CMake). Files are compiled individually with g++:

```bash
g++ -o output_name file.cpp        # basic compile
g++ -std=c++17 -o output file.cpp  # when using modern features
g++ -lm -o output file.cpp         # when using math library (graphics programs)
```

Some graphics programs use POSIX APIs (`sys/ioctl.h`, `unistd.h`) and `-std=c++17` threads — these are Linux-only.

## Repository Structure

- **hw/** and **lab/** — Course assignments (CS coursework); numbered sequentially
- **projects/leetcode/** — Problem solutions, standalone single-file programs
- **projects/deep_dives/** — Explorations of hardware/binary-level topics; compiled binaries often live alongside .cpp files
- **projects/graphics/** — Terminal-based visual programs (sine waves, cubes, colors)
- **projects/fun/** — Misc programs (truth tables, binary converter)
- **idk/** — Scratch/experimentation space

## Code Conventions

- Files are self-contained single-file programs with `main()` (no multi-file builds)
- `using namespace std;` is used throughout
- Inline comments explain reasoning and learning insights, not just what the code does
- Compiled binaries are committed alongside source in some directories (not gitignored except .o and .out)
