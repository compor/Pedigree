
/* whale book - figure 8.21 */

/*
 * clang -c -emit-llvm file.c
 * opt -mem2reg -simplifycfg
 *
 */

#include <stdio.h>

void foo(int n) {
  int k = 0;
  int i = 1, j = 2;

  while (i <= n) {
    j *= 2;
    k = 1;
    i += 1;
  }

  if (k)
    printf("%d\n", j);
  else
    i += 1;

  return;
}
