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

| `dev` | [Alpine Linux](https://alpinelinux.org/) | [Ubuntu](https://ubuntu.com/) |
|---|---|---|
| [![clang++](https://img.shields.io/badge/clang++-%3E=10-lightgray)](https://clang.llvm.org/) | [![build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml/badge.svg?branch=dev)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml?query=branch%3Adev) | [![build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml/badge.svg?branch=dev)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml?query=branch%3Adev) |
| [![g++](https://img.shields.io/badge/g++-%3E=10-lightgray)](https://gcc.gnu.org/) | [![build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml/badge.svg?branch=dev)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml?query=branch%3Adev) | [![build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml/badge.svg?branch=dev)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml?query=branch%3Adev) |

| `master` | [Alpine Linux](https://alpinelinux.org/) | [Ubuntu](https://ubuntu.com/) |
|---|---|---|
| [![clang++](https://img.shields.io/badge/clang++-%3E=10-lightgray)](https://clang.llvm.org/) | [![build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml/badge.svg?branch=master)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml?query=branch%3Amaster) | [![build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml/badge.svg?branch=master)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml?query=branch%3Amaster) |
| [![g++](https://img.shields.io/badge/g++-%3E=10-lightgray)](https://gcc.gnu.org/) | [![build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml/badge.svg?branch=master)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml?query=branch%3Amaster) | [![build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml/badge.svg?branch=master)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml?query=branch%3Amaster) |

## Build and Install

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make && sudo make install
```

## Example

Explore simple server example [here](example).
