## xalwart.server

[![c++](https://img.shields.io/badge/c%2B%2B-20-6c85cf)](https://isocpp.org/)
[![cmake](https://img.shields.io/badge/cmake-%3E=3.12-success)](https://cmake.org/)
[![alpine](https://img.shields.io/badge/Alpine_Linux-0D597F?style=flat&logo=alpine-linux&logoColor=white)](https://alpinelinux.org/)
[![ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=flat&logo=ubuntu&logoColor=white)](https://ubuntu.com/)

## Requirements

The following compilers are tested with the CI system, and are known to work
on Alpine Linux and Ubuntu.
* g++ 10 or later
* clang++ 10 or later

To build the library from source CMake 3.12 or later is required.

### Dependencies

The following library is required:
- [xalwart.core](https://github.com/YuriyLisovskiy/xalwart.core)

## Build Status

* ### clang++
    [![Test Alpine Linux](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml) [![Test Ubuntu](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml)

* ### g++
    [![Test Alpine Linux](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml) [![Test Ubuntu](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml)

## Build and Install

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make && sudo make install
```

## Example

Explore simple server example [here](example).
