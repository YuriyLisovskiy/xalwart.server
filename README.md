## xalwart.server [![c++](https://img.shields.io/badge/c%2B%2B-20-6c85cf)](https://isocpp.org/)

| [![alpine](https://img.shields.io/badge/Alpine_Linux-0D597F?style=for-the-badge&logo=alpine-linux&logoColor=white)](https://alpinelinux.org/) | [![ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)](https://ubuntu.com/) | [![macOS](https://img.shields.io/badge/mac_OS-gray?style=for-the-badge&logo=apple&logoColor=white)](https://www.apple.com/macos/) | [![windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)](https://www.microsoft.com/en-us/windows/) |
|:---:|:---:|:---:|:---:|
| [![clang](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml) | [![clang](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml) | *not supported* | *not supported* |
| [![gcc](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml) | [![gcc](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml) | *not supported* | *not supported* |

### Requirements
- C++ compiler:
    - `gcc`: v10 or later.
    - `clang`: v10 or later.
- CMake: `3.12` or later.
- Additional libraries:
    - [xalwart.core](https://github.com/YuriyLisovskiy/xalwart.core) (required)

### Build and Install
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make && sudo make install
```

### Testing
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. && make
```
Use valgrind to check for memory leaks:
```bash
valgrind --leak-check=full ./tests/unittests-all
```
