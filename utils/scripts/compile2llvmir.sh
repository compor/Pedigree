#!/usr/bin/env bash

for CUR_FILE in "${@}"; do 
  FILE_PREFIX=${CUR_FILE%.*}
  PREFIX_LEN=${#FILE_PREFIX}
  
  PREFIX_OFFSET=$(echo "${PREFIX_LEN}-1" | bc)

  BITCODE_FILENAME="${CUR_FILE:0:${PREFIX_LEN}}.ll"

  clang -c -S -emit-llvm ${CUR_FILE} -o ${BITCODE_FILENAME}
  #clang -g -c -S -emit-llvm ${CUR_FILE} -o ${BITCODE_FILENAME}

  [[ $? -eq 0 ]] && echo "Compiled ${CUR_FILE} to ${BITCODE_FILENAME}"
done
