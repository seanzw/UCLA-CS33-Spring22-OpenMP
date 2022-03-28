
seq: main.c kernel_seq.c
	gcc $^ -DKERNEL=kernel_seq -std=c99 -O3 -o $@

omp: main.c kernel_seq.c kernel_omp.c
	gcc $^ -DOPENMP -std=c99 -fopenmp -O3 -o $@

check: main.c kernel_seq.c kernel_omp.c
	gcc $^ -DOPENMP -DCHECK -std=c99 -fopenmp -O3 -o $@