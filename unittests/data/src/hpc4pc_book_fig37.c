/* high performance compilers for parallel computing book by Michael Wolfe -
 * figure 7.3 */

/*
 * clang -c -emit-llvm file.c
 * opt -mem2reg
 *
 */

void foo() {
  int k = 0;
  int i = 0;

b_label:
  k += 3;

  while (k < 10) {
    while (i < 30) {
      if (i < 5)
        i += 2;
      else
        i += 3;

      i++;
    }

    k++;
  }

i_label:
  k += 7;

j_label:
  k += 4;
}
