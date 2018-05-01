# pedigree - Program Dependence Graph pass

## Introduction

This is a `LLVM` [`opt`][1] pass for creating Program Dependence Graphs.

## Features

-   Out-of-source builds thanks to [cmake][2].
-   In-tree `LLVM` builds.

## Requirements

-   [cmake][2] 3.2.0 or later
-   C++ compilers:
    -   [LLVM][3]  
        tested with:
        -   3.7.x
        -   6.0.0
    -   [GCC][4]  
        tested with:
        -   5.4.1
    -   Boost
        tested with:
        -   1.59
-   [googletest][6]  
    tested with:
    -   1.8.0

## External dependencies

### Mandatory

-   [cmake-utils][9]  
    Used for supporting the `cmake`-based build system. Using `git clone --recursive` should take care of everything.
-   [sanitizers-cmake][12]
    Used to provide support for sanitizers. Although the subproject is required for successful configuration, its use is
    optional.

### Optional

None.

## How to build

1.  `git clone --recursive` this repo.
2.  Create a directory for an out-of-source build and `cd` into it.
3.  Run `cmake` and `cmake --build .` with that appropriate options.
    For examples on the various options have a look at the build scripts (provided for convenience) located in the
    `utils/scripts/build` subdirectory.

-   compiler selection is catered by the `exports_local_*` scripts using the `CC` and `CXX` variables for my current 
    machine, so adjust appropriately for your setup.
-   export one of the `exports_deps_*` scripts, depending on the kind of setup you are interested in.

4.  Optionally, you can install the pass by

    `cmake -DCMAKE_INSTALL_PREFIX=[path-to-install] -P cmake_install.cmake`

    Omitting `CMAKE_INSTALL_PREFIX` will use the `../install/` directory relative to the build directory.

5.  [`lit`][7]-based tests can be run with `cmake --build . --target check`
6.  [googletest][6]-based unit tests can be run with `./run_unit_tests.sh` (see the script for details).

## How to execute

### Using opt

`opt -load [path to plugin]/libLLVMPedigreePass.so -pedigree foo.bc -o foo.out.bc`

### Using clang

`clang -Xclang -load -Xclang [path to plugin]/libLLVMPedigreePass.so foo.c -o foo`

## Notes

### Customizing

A first step towards customization is to search for the pattern `[Ss]keleton]` and replace as with the desired name. A
general good approach is to prefix everything with the name of the pass, especially in languages that do not offer some
sort of namespace capability like `cmake`.

Another customization that needs to be take care of in the case of creating a new project based on this template is the
use of [cmake-utils][9]. There are 3 options:

-   Just copy the files and include them in the new repository.
-   Use it as a submodule using the initial URL address.
-   Use it as a submodule using a fork of the initial URL address. This allows you to keep the repo separately and have
    full control on updates and other desired enhancements.

## Clarifications on building

The shared object containing the pass is dynamically loaded by `opt`. This poses a restriction on its build dependency
upon `opt` and its transient dependency to the `C++` library it is linked against. So, it is recommended to perform
`readelf -d $(which opt)` and note which implementation of the `C++` library it uses 
([`libc++`][10] or [`libstdc++`][11]). Then, you can influence which `C++` library your pass uses with the
`CMAKE_CXX_FLAGS` and the `-stdlib` flag for `LLVM`. 

The main problem stems from the binary incompatibility (ABI) between the two implementations, although they are API
compatible since they adhere to the ISO standard. For this reason, `libc++` uses the inlined namespace `std::__1`, which
will show up in the errors when you have your pass built against `libstdc++` and try to load it using an `opt` built
against `libc++`.

When `LLVM` is build with `BUILD_SHARED_LIBS=OFF`, `opt` cannot load dynamically any pass built as a shared object,
complaining about multiply defined symbols, since it contains everything in its executable. The only workaround is to
build your pass in the `LLVM` tree (resulting in a static archive, also included in the `opt` executable).

### Miscellaneous

When the build script uses `LLVM` `cmake` [utility][8] functions the `lib` shared library prefix is omitted.

### TODO

-   Add `travis-ci` support.

[1]: http://llvm.org/docs/WritingAnLLVMPass.html

[2]: https://cmake.org

[3]: http://www.llvm.org

[4]: https://gcc.gnu.org

[6]: https://github.com/google/googletest

[7]: https://llvm.org/docs/CommandGuide/lit.html

[8]: http://llvm.org/docs/CMake.html#cmake-out-of-source-pass

[9]: https://github.com/compor/cmake-utils

[10]: https://libcxx.llvm.org/docs/

[11]: https://gcc.gnu.org/wiki/Libstdc++

[12]: https://github.com/arsenm/sanitizers-cmake
