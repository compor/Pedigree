/*
 * clang -c -emit-llvm file.c
 * reduced test case via bugpoint
 *
 */

#include <stddef.h>

extern int initialize();

void foo() {
  int A[10] = {0};
  size_t i;

  for (i = 0; i < sizeof(A); ++i)
    A[i] = initialize();
}
