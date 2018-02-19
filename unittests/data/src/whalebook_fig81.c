/* whale book - figure 8.1 */

/*
 * clang -c -emit-llvm file.c
 * opt -mem2reg -simplifycfg
 *
 */

int goo(int m, int i);

int foo(int n) {
  int i = 0, j;

  if (n == 1)
    i = 2;

  while (n > 0) {
    j = i + 1;
    n = goo(n, i);
  }

  return j;
}
