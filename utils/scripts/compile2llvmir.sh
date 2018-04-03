#!/usr/bin/env bash

declare -a TMP_FILES

function cleanup_tmp() {
  echo "Cleaning up temp files..."

  for tmpfile in "${TMP_FILES[@]}"; do
    rm -f "${tmpfile}"
  done
}

trap cleanup_tmp EXIT

for CUR_FILE in "${@}"; do 
  FILE_PREFIX=${CUR_FILE%.*}
  PREFIX_LEN=${#FILE_PREFIX}
  
  BITCODE_FILENAME="${CUR_FILE:0:${PREFIX_LEN}}.ll"
  TMP_FILE="tmp.${BITCODE_FILENAME}"

  TMP_FILES+=( ${TMP_FILE} )

  clang -c -S -emit-llvm "${CUR_FILE}" -o "${TMP_FILE}"
  [[ ${?} -ne 0 ]] && exit 1
  echo "Compiled ${CUR_FILE} to ${TMP_FILE}"

  opt -S -mem2reg "${TMP_FILE}" -o "${BITCODE_FILENAME}"
  [[ ${?} -ne 0 ]] && exit 1
  echo "Compiled ${BITCODE_FILENAME} to ${TMP_FILE}"
done
