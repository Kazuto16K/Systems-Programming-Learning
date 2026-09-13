# Systems Projects

A collection of small, from-scratch C++ programs written to build up
systems-level C++ skills: manual memory management, RAII, move semantics,
custom data structures, binary file I/O, multithreading, memory-mapped
files, and raw sockets — the foundations needed to build real systems
software without leaning on high-level abstractions.

Everything here is written by hand as a learning exercise, not
AI-generated or copied from tutorials.

## Structure

```
day01/   pointer lifetime & memory management
day02/   RAII & smart pointers
day03/   move semantics & the Rule of Five
...
```

Each `dayNN/` folder is a small, self-contained program focused on one
concept, building in difficulty from basic pointer/memory handling up
through templates, hashing, threading, and networking.

## Capstone

After the daily exercises, a larger incremental project (`sysmon/`) ties
several of these concepts together into a live terminal process monitor
for Windows — process listing, CPU/memory sampling, a background sampling
thread, and a live-updating dashboard.

## Building

Each exercise is a standalone `.cpp` file (or has its own `CMakeLists.txt`
once the project grows past a single file). Compile individually, e.g.:

```
g++ -std=c++17 -o day01/pointer day01/pointer.cpp
```
