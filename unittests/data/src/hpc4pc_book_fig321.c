/* high performance compilers for parallel computing book by Michael Wolfe -
 * figure 3.21 */

/*
 * clang -c -emit-llvm file.c
 * opt -mem2reg
 *
 */

void foo() {
  int k = 0;
  int i = 0;

  if (k > 0) {
    while (k < 15) {
      if (k < 10)
        k++;
      else
        k += 3;

      k++;
    }

  g_label:
    k += 2;
  }

h_label:
  k++;
}
