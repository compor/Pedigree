/* whale book - figure 8.5 */

/*
 * clang -c -emit-llvm file.c
 * opt -mem2reg -simplifycfg
 *
 */

void foo(int z) {
  int x = 0, y = 0;

  if (z > 1) {
    x = 1;

    if (z > y) {
      y = x + 1;
      return;
    }
  } else
    x = 2;

  z = x - 3;

  return;
}
