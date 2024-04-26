#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int my_rank, nr_procs;
    const int root = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procs);
    int n;
    if (my_rank == root) {
        if (argc < 2) {
            fprintf(stderr, "Usage: %s n\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        n = atoi(argv[1]);
    }
    MPI_Bcast(&n, 1, MPI_INT, root, MPI_COMM_WORLD);
    int lb, ub, data_size;
    data_size = (n - 1)/nr_procs + 1;
    lb = my_rank*data_size;
    if (lb < n) {
        if (lb + data_size >= n) {
            ub = n - 1;
            data_size = ub - lb + 1;
        } else {
            ub = lb + data_size - 1;
        }
    } else {
            lb = ub = -1;
            data_size = 0;
    }
    printf("Process %d: lb=%d, ub=%d, data_size=%d\n", my_rank, lb, ub, data_size);
    MPI_Finalize();
    return 0;
}
