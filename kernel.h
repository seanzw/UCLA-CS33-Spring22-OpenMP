#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

inline int maximum(int a, int b, int c) {
  int k = a <= b ? b : a;
  return k <= c ? c : k;
}

// The kernel definition.
void kernel_omp(int *input, int *ref, int64_t cols, int64_t rows, int penalty);
void kernel_seq(int *input, int *ref, int64_t cols, int64_t rows, int penalty);

#endif