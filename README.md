# Multi population evolution (mpev)

## Overview

An evolutionary computing framework to support experimentation with coevolution
and multiple populations in particular. Being developed, in part, to support
my final year project at the University of York.

## Building

Uses C++17
CMakeLists.txt will download googletest (https://github.com/google/googletest)

Tested on MacOS High Sierra with Clang 5.0.0 installed from brew

```$bash
mkdir build && cd build && cmake -D CMAKE_CXX_COMPILER=/usr/local/Cellar/llvm/5.0.0/bin/clang++ -D CMAKE_C_COMPILER=/usr/local/Cellar/llvm/5.0.0/bin/clang ..
```

## To do
* Store the random seed in the context and use a common generator across all
things that need randomness.
* Support reading in a TOML file to run multiple configurations with different
parameters
* Display progress information on std::out
* Tidy up & improve tests
* Add more reporting mechanisms, eg. report the absolute & relative fitness of
each individual
* Add concurrency support. I plan on doing this with Folly futures
(https://github.com/facebook/folly).
