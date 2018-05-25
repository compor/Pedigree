/* lenherr2008
 * figure 4 */

/*
 * clang -c -emit-llvm file.c
 *
 */

extern int condition();

void foo3() {
  int a, b;
  int *p1, *p2, *p3;
  int **dp;

  p1 = &a;
  dp = &p1;

  while (condition()) {
    p2 = p1;
    *dp = &b;
  }

  p3 = *dp;
}
