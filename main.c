#include <assert.h>
#include <malloc.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "kernel.h"

#ifndef KERNEL
#define KERNEL kernel_omp
#endif

// Returns the current system time in microseconds
long long get_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}

#include "blosum64.h"

double gettime() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec + t.tv_usec * 1e-6;
}

void usage(int argc, char **argv) {
  fprintf(stderr, "Usage: %s <len> <penalty> <num_threads>\n", argv[0]);
  fprintf(stderr, "\t<len>            - length of seq (multiple of 64) \n");
  fprintf(stderr, "\t<num_threads>    - no. of threads\n");
  exit(1);
}

int main(int argc, char **argv) {

  // the lengths of the two sequences should be able to divided by 16.
  // And at current stage  max_rows needs to equal max_cols
  int len, omp_num_threads;
  if (argc == 3) {
    len = atoi(argv[1]);
    omp_num_threads = atoi(argv[2]);
  } else {
    usage(argc, argv);
  }

  const int penalty = 5;

  if (len % 64 != 0) {
    fprintf(stderr, "error: len should be multiple of 64.");
    exit(1);
  }

  if (omp_num_threads <= 0 || omp_num_threads > 32) {
    fprintf(stderr, "Number of threads should be 1-32.");
    exit(1);
  }

  int max_rows = len + 1;
  int max_cols = len + 1;
  int *ref = (int *)malloc(max_rows * max_cols * sizeof(int));
  int *input = (int *)malloc(max_rows * max_cols * sizeof(int));

  if (!input || !ref) {
    fprintf(stderr, "error: can not allocate memory");
    exit(1);
  }

  srand(7);

  memset(input, 0, sizeof(int) * max_rows * max_cols);

  // Randomize the input matrix.
  for (uint64_t i = 1; i < max_rows; i++) {
    input[i * max_cols] = rand() % 10 + 1;
  }
  for (uint64_t j = 1; j < max_cols; j++) {
    input[j] = rand() % 10 + 1;
  }

  // Generate the reference matrix.
  for (uint64_t i = 1; i < max_cols; i++) {
    for (uint64_t j = 1; j < max_rows; j++) {
      ref[i * max_cols + j] = blosum62[input[i * max_cols]][input[j]];
    }
  }

  for (uint64_t i = 1; i < max_rows; i++) {
    input[i * max_cols] = -i * penalty;
  }
  for (uint64_t j = 1; j < max_cols; j++) {
    input[j] = -j * penalty;
  }

// Set number of threads.
#ifdef OPENMP
  omp_set_num_threads(omp_num_threads);
#else
  omp_num_threads = 1;
#endif

#ifdef CHECK
  int *truth = malloc(max_rows * max_cols * sizeof(int));
  memcpy(truth, input, sizeof(int) * max_rows * max_cols);
  kernel_seq(truth, ref, max_rows, max_cols, penalty);
#endif

  printf("Num of threads: %d. Matrix size %dx%d\n", omp_num_threads, max_rows,
         max_cols);

  long long start_time = get_time();
  KERNEL(input, ref, max_rows, max_cols, penalty);
  long long end_time = get_time();

  printf("Total time: %ld us\n", (end_time - start_time));

#ifdef CHECK
  for (int i = 0; i < max_rows; ++i) {
    for (int j = 0; j < max_cols; ++j) {
      int64_t idx = i * max_cols + j;
      if (truth[idx] != input[idx]) {
        fprintf(stderr, "Wrong result at %dx%d %d != %d.\n", i, j, truth[idx],
                input[idx]);
        exit(1);
      }
    }
  }
  printf("All result match!\n");
#endif

  free(ref);
  free(input);
#ifdef CHECK
  free(truth);
#endif

  return 0;
}
