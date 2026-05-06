#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

typedef struct {
    int lb;
    int ub;
} range_t;

range_t compute_chunk(MPI_Comm comm, int n) {
    range_t range;
    int my_rank, nr_procs;
    MPI_Comm_rank(comm, &my_rank);
    MPI_Comm_size(comm, &nr_procs);
    int data_size = n/nr_procs;
    int num_extra = n % nr_procs;
    if (my_rank < num_extra) {
        data_size++;
        range.lb = my_rank*data_size;
    } else if (data_size > 0) {
        range.lb = my_rank*data_size + num_extra;
    } else {
        range.lb = -1;
        data_size = 0;
    }
    range.ub = range.lb + data_size - 1;
    return range;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int my_rank, nr_procs;
    const int root = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procs);
    int n = 10000000;
    if (my_rank == root && argc > 1) {
        n = atoi(argv[1]);
    }
    MPI_Bcast(&n, 1, MPI_INT, root, MPI_COMM_WORLD);
    range_t range = compute_chunk(MPI_COMM_WORLD, n);
    double p_sum = 0.0;
    for (int i = range.lb; i < range.ub; i++) {
        double x = (i + 0.5)/n;
        p_sum += 4.0/(1.0 + x*x);
    }
    double sum;
    MPI_Reduce(&p_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);
    if (my_rank == root) {
        double pi = sum/n;
        printf("pi = %.15f\n", pi);
    }
    MPI_Finalize();
    return 0;
}
