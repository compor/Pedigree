/* lenherr2008
 * figure 1 */

/*
 * clang -c -emit-llvm file.c
 *
 */

extern int condition();
extern int *initialize();

int foo2() {
  int *x = initialize();
  int *y = initialize();

  *x = 42;

  if (condition()) {
    *y = 0;
  }

  return *x;
}
