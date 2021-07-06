## xalwart.server [![c++](https://img.shields.io/badge/c%2B%2B-20-6c85cf)](https://isocpp.org/) [![c++](https://img.shields.io/badge/cmake-%3E=3.12-success)](https://cmake.org/)

| `dev` | [![alpine](https://img.shields.io/badge/Alpine_Linux-0D597F?style=flat&logo=alpine-linux&logoColor=white)](https://alpinelinux.org/) | [![ubuntu](https://img.shields.io/badge/Ubuntu-E95420?style=flat&logo=ubuntu&logoColor=white)](https://ubuntu.com/) | [![macOS](https://img.shields.io/badge/macOS-gray?style=flat&logo=apple&logoColor=white)](https://www.apple.com/macos/) | [![windows](https://img.shields.io/badge/Windows-0078D6?style=flat&logo=windows&logoColor=white)](https://www.microsoft.com/en-us/windows/) |
|:---:|:---:|:---:|:---:|:---:|
| [![clang](https://img.shields.io/badge/clang-%3E=10-lightgray)](https://clang.llvm.org/) | [![clang-build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml/badge.svg?branch=dev)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml?query=branch%3Adev) | [![clang-build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml/badge.svg?branch=dev)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml?query=branch%3Adev) | *not supported* | *not supported* |
| [![gcc](https://img.shields.io/badge/gcc-%3E=10-lightgray)](https://gcc.gnu.org/) | [![gcc-build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml/badge.svg?branch=dev)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml?query=branch%3Adev) | [![gcc-build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml/badge.svg?branch=dev)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml?query=branch%3Adev) | *not supported* | *not supported* |
| **`master`** |
| [![clang](https://img.shields.io/badge/clang-%3E=10-lightgray)](https://clang.llvm.org/) | [![clang-build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml/badge.svg?branch=master)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-alpine.yml?query=branch%3Amaster) | [![clang-build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml/badge.svg?branch=master)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-clang-ubuntu.yml?query=branch%3Amaster) | *not supported* | *not supported* |
| [![gcc](https://img.shields.io/badge/gcc-%3E=10-lightgray)](https://gcc.gnu.org/) | [![gcc-build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml/badge.svg?branch=master)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-alpine.yml?query=branch%3Amaster) | [![gcc-build](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml/badge.svg?branch=master)](https://github.com/YuriyLisovskiy/xalwart.server/actions/workflows/tests-gcc-ubuntu.yml?query=branch%3Amaster) | *not supported* | *not supported* |

### Dependencies
  - [xalwart.core](https://github.com/YuriyLisovskiy/xalwart.core) (required)

### Build and Install
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make && sudo make install
```

### Example
Explore simple server example [here](example).
