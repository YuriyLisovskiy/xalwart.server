## xalwart.server
[![c++](https://img.shields.io/badge/c%2B%2B-20-6c85cf)](https://isocpp.org/)
[![cmake](https://img.shields.io/badge/cmake-%3E=3.12-success)](https://cmake.org/)
[![alpine](https://img.shields.io/badge/Alpine_Linux-0D597F?style=flat&logo=alpine-linux&logoColor=white)](https://alpinelinux.org/)
[![ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=flat&logo=ubuntu&logoColor=white)](https://ubuntu.com/)

## Build Status
| @ | Build |
|---|---|
| Dev branch: | [![Build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/build.yml/badge.svg?branch=dev)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/build.yml?query=branch%3Adev) |
| Master branch: | [![Build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/build.yml?query=branch%3Amaster) |

## Requirements
The following compilers are tested with the CI system, and are known to work
on Alpine Linux and Ubuntu.
* g++ 10 or later
* clang++ 10 or later

To build the library from source CMake 3.12 or later is required.

### Dependencies
The following library is required:
- [xalwart.base](https://github.com/YuriyLisovskiy/xalwart.base) 0.x.x or later

## Build and Install
* `BUILD_SHARED_LIB` means to build a shared or static library (`off` by default).
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIB=off ..
make

# for linux:
sudo make install
```

## Example
Explore simple server example [here](example).
