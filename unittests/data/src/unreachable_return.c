/*
 * clang -c -emit-llvm file.c
 *
 */

#include <stdlib.h>

#define N 5

void foo() {
  int i;
  for (i = 0; i < N; i++) {
  }
  exit(0);
}

