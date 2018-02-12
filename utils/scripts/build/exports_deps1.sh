#!/usr/bin/env bash

# LLVM/Clang should be in the PATH for this to work
export CC=clang
export CXX=clang++
export LLVMCONFIG=llvm-config

# or picked up from a system package install
if [[ ! -z ${COMPILER_VERSION} ]]; then
  export CC=${CC}-${COMPILER_VERSION}
  export CXX=${CXX}-${COMPILER_VERSION}
  export LLVMCONFIG=${LLVMCONFIG}-${COMPILER_VERSION}
fi

COMPILER_VERSION=

export BUILD_TYPE=Debug

export GTEST_ROOT=/usr/local/gtest-libcxx

export CXX_FLAGS=
export CXX_FLAGS="-stdlib=libc++"

export LINKER_FLAGS=
export LINKER_FLAGS="-Wl,-L$(${LLVMCONFIG} --libdir)" 
export LINKER_FLAGS="${LINKER_FLAGS} -lc++ -lc++abi" 

export PEDIGREE_SKIP_TESTS=OFF
export PEDIGREE_DEBUG=ON


# find LLVM's cmake dir
export LLVM_DIR=$(${LLVMCONFIG} --prefix)/share/llvm/cmake/

# versions 3.8 and up provide a flag for it
${LLVMCONFIG} --cmakedir &> /dev/null
[[ $? -eq 0 ]] && export LLVM_DIR=$(${LLVMCONFIG} --cmakedir)


CMAKE_OPTIONS="-DLLVM_DIR=${LLVM_DIR}"
CMAKE_OPTIONS="${CMAKE_OPTIONS} -DGTEST_ROOT=${GTEST_ROOT}"
CMAKE_OPTIONS="${CMAKE_OPTIONS} -DPEDIGREE_SKIP_TESTS=${PEDIGREE_SKIP_TESTS}"
CMAKE_OPTIONS="${CMAKE_OPTIONS} -DPEDIGREE_DEBUG=${PEDIGREE_DEBUG}"

export CMAKE_OPTIONS

