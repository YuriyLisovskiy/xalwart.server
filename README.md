## xalwart.server
[![gcc](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc.yml)
[![clang](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang.yml/badge.svg)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang.yml)

### Requirements
- `C++20`
- C++ compiler:
    - `gcc`: v10 or later.
    - `clang`: v10 or later.
- CMake: `3.12` or later.
- Additional libraries:
    - [xalwart.core](https://github.com/YuriyLisovskiy/xalwart.core)

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
