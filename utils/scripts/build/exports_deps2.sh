#!/usr/bin/env bash

# LLVM/Clang should be in the PATH for this to work
export CC=clang
export CXX=clang++
export LLVMCONFIG=llvm-config

#COMPILER_VERSION=3.8

# or picked up from a system package install
if [[ ! -z ${COMPILER_VERSION} ]]; then
  export CC=${CC}-${COMPILER_VERSION}
  export CXX=${CXX}-${COMPILER_VERSION}
  export LLVMCONFIG=${LLVMCONFIG}-${COMPILER_VERSION}
fi

export BUILD_TYPE=Debug

export GTEST_ROOT=/usr/local/gtest-libstdcxx

export BOOST_ROOT=/bulk/workbench/boost/015900/install/

export CXX_FLAGS=
export CXX_FLAGS="${CXX_FLAGS} -O1"

export LINKER_FLAGS=

export PEDIGREE_SKIP_TESTS=OFF


# find LLVM's cmake dir
export LLVM_DIR=$(${LLVMCONFIG} --prefix)/share/llvm/cmake/

# versions 3.8 and up provide a flag for it
${LLVMCONFIG} --cmakedir &> /dev/null
[[ $? -eq 0 ]] && export LLVM_DIR=$(${LLVMCONFIG} --cmakedir)


CMAKE_OPTIONS="-DLLVM_DIR=${LLVM_DIR}"
CMAKE_OPTIONS="${CMAKE_OPTIONS} -DGTEST_ROOT=${GTEST_ROOT}"
CMAKE_OPTIONS="${CMAKE_OPTIONS} -DBOOST_ROOT=${BOOST_ROOT}"
CMAKE_OPTIONS="${CMAKE_OPTIONS} -DPEDIGREE_SKIP_TESTS=${PEDIGREE_SKIP_TESTS}"

export CMAKE_OPTIONS

