# Multi population evolution (mpev)

## Overview

An evolutionary computing framework to support experimentation with coevolution
and multiple populations in particular. Being developed, in part, to support
my final year project at the University of York.

## Building

Uses C++17
CMakeLists.txt will download googletest (https://github.com/google/googletest)

[cpptoml](https://github.com/skystrife/cpptoml)

Tested on MacOS High Sierra with Clang 5.0.0 installed from brew

```$bash
mkdir build && cd build && cmake -D CMAKE_CXX_COMPILER=/usr/local/Cellar/llvm/5.0.0/bin/clang++ -D CMAKE_C_COMPILER=/usr/local/Cellar/llvm/5.0.0/bin/clang ..
```

## To do
* Display progress information on std::out
* Tidy up & improve tests
