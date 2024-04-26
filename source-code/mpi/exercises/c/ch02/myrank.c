#include <mpi.h>
#include <stdio.h>

int main() {
    MPI_Init(NULL, NULL);
    int my_rank, nr_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procs);
    if (my_rank == 0) {
        printf("Hello, World!\n");
    }
    printf("I am process %d of %d\n", my_rank, nr_procs);
    MPI_Finalize();
    return 0;
}
