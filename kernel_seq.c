#include <omp.h>

#include "kernel.h"

void kernel_seq(int *input, int *ref, int64_t rows, int64_t cols, int penalty) {

  for (int i = 1; i < rows; ++i) {
    for (int j = 1; j < cols; ++j) {

      int64_t idx = i * cols + j;

      int64_t idxNW = idx - cols - 1;
      int64_t idxN = idx - cols;
      int64_t idxW = idx - 1;
      int r = ref[idx];
      int inputNW = input[idxNW];
      int inputW = input[idxW];
      int inputN = input[idxN];

      input[idx] = maximum(inputNW + r, inputW - penalty, inputN - penalty);
    }
  }
}
