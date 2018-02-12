#!/usr/bin/env bash

[[ $TRAVIS == "true" ]] && set -evx

[[ ! $TRAVIS == "true" ]] && [[ -z ${LLVMCONFIG} ]] && echo "LLVMCONFIG is not set" && exit 1

SRC_DIR=$1
INSTALL_DIR=$2

LINKER_FLAGS="-Wl,-L$(${LLVMCONFIG} --libdir) -lc++ -lc++abi"

LD_LIBRARY_PATH="$(${LLVMCONFIG} --libdir)" \
cmake \
  -DCMAKE_CXX_FLAGS="-stdlib=libc++" \
  -DCMAKE_SHARED_LINKER_FLAGS="${LINKER_FLAGS}" \
  -DCMAKE_EXE_LINKER_FLAGS="${LINKER_FLAGS}" \
  -DCMAKE_MODULE_LINKER_FLAGS="${LINKER_FLAGS}" \
  -DCMAKE_POLICY_DEFAULT_CMP0022=NEW \
  -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
  ${SRC_DIR}

