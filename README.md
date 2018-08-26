# sparkle

[![C++14](https://img.shields.io/badge/C++-14-4c7e9f.svg)](http://en.cppreference.com/w/cpp)
[![Build Status](https://travis-ci.org/uchuhimo/sparkle.svg?branch=master)](https://travis-ci.org/uchuhimo/sparkle)

An actor system for C++.

## Prerequisites

- CMake 2.8 or higher
- GCC 5 or higher
- Conan

## Build from source

Debug:

```bash
conan install . -s build_type=Debug --install-folder=cmake-build-debug
cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug .. && make
```

Release:

```bash
conan install . -s build_type=Release --install-folder=cmake-build-release
cd cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release .. && make
```

# License

© uchuhimo, 2017-2018. Licensed under an [Apache 2.0](./LICENSE) license.
