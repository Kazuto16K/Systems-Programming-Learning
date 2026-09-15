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
day12/   review + persistent key-value store warm-up
```

Each `dayNN/` folder is a small, self-contained program focused on one
concept, building in difficulty from basic pointer/memory handling up
through templates, hashing, threading, and networking, across 12 sessions.

## Capstone — minidis

After the daily exercises, a larger incremental project (`minidis/`) ties
several of these concepts together into a small in-memory key-value server
in the shape of Redis: a TCP server with a custom command protocol,
thread-per-connection concurrency, TTL/expiry, and crash-safe persistence
via a snapshot + append-only write log. Built over 7 sessions.

## Building

Each exercise is a standalone `.cpp` file (or has its own `CMakeLists.txt`
once the project grows past a single file). Compile individually, e.g.:

```
g++ -std=c++17 -o day01/pointer day01/pointer.cpp
```
