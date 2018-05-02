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

export GTEST_ROOT=/usr/local/gtest-libcxx-msan

export BOOST_ROOT=/bulk/workbench/boost/015900/install/

LIBCXX_MSAN_ROOT=/bulk/workbench/llvm/6.0/libcxx-msan/

MSAN_COMPILE_FLAGS=""
MSAN_COMPILE_FLAGS="${MSAN_COMPILE_FLAGS} -fsanitize=memory"
MSAN_COMPILE_FLAGS="${MSAN_COMPILE_FLAGS} -I${LIBCXX_MSAN_ROOT}/include"
MSAN_COMPILE_FLAGS="${MSAN_COMPILE_FLAGS} -I${LIBCXX_MSAN_ROOT}/include/c++/v1"

export CXX_FLAGS=
export CXX_FLAGS="${CXX_FLAGS} -O1"
export CXX_FLAGS="${CXX_FLAGS} -stdlib=libc++"
export CXX_FLAGS="${CXX_FLAGS} -nostdinc++"
export CXX_FLAGS="${CXX_FLAGS} ${MSAN_COMPILE_FLAGS}"

MSAN_LINKER_FLAGS=
MSAN_LINKER_FLAGS="${MSAN_LINKER_FLAGS} -Wl,-L${LIBCXX_MSAN_ROOT}/lib"
MSAN_LINKER_FLAGS="${MSAN_LINKER_FLAGS} -Wl,-rpath ${LIBCXX_MSAN_ROOT}/lib/"
#MSAN_LINKER_FLAGS="${MSAN_LINKER_FLAGS} -Wl,-rpath-link ${LIBCXX_MSAN_ROOT}/lib/"

export LINKER_FLAGS=
export LINKER_FLAGS="${LINKER_FLAGS} -lc++ -lc++abi"
#export LINKER_FLAGS="${LINKER_FLAGS} -lm -lc"
export LINKER_FLAGS="${LINKER_FLAGS} ${MSAN_LINKER_FLAGS}"

export PEDIGREE_SKIP_TESTS=OFF

export SANITIZER_OPTIONS=""
export SANITIZER_OPTIONS="${SANITIZER_OPTIONS} -DSANITIZE_MEMORY=ON"
export SANITIZER_OPTIONS="${SANITIZER_OPTIONS} -DSANITIZE_UNDEFINED=ON"

# find LLVM's cmake dir
export LLVM_DIR=$(${LLVMCONFIG} --prefix)/share/llvm/cmake/

# versions 3.8 and up provide a flag for it
${LLVMCONFIG} --cmakedir &> /dev/null
[[ $? -eq 0 ]] && export LLVM_DIR=$(${LLVMCONFIG} --cmakedir)


CMAKE_OPTIONS="-DLLVM_DIR=${LLVM_DIR}"
CMAKE_OPTIONS="${CMAKE_OPTIONS} -DGTEST_ROOT=${GTEST_ROOT}"
CMAKE_OPTIONS="${CMAKE_OPTIONS} -DBOOST_ROOT=${BOOST_ROOT}"
CMAKE_OPTIONS="${CMAKE_OPTIONS} -DPEDIGREE_SKIP_TESTS=${PEDIGREE_SKIP_TESTS}"
CMAKE_OPTIONS="${CMAKE_OPTIONS} ${SANITIZER_OPTIONS}"

export CMAKE_OPTIONS

