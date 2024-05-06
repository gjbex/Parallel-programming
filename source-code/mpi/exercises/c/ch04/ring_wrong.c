#include <stdio.h>
#include <mpi.h>

int main() {
    const int TAG = 17;
    MPI_Init(NULL, NULL);
    int my_rank, nr_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procs);
    int buffer = my_rank;
    int dest = (my_rank + 1) % nr_procs;
    int source = (my_rank - 1 + nr_procs) % nr_procs;
    int sum = 0;
    for (int i = 0; i < nr_procs; i++) {
        MPI_Send(&buffer, 1, MPI_INT, dest, TAG, MPI_COMM_WORLD);
        MPI_Recv(&buffer, 1, MPI_INT, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        sum += buffer;
    }
    printf("rrank %d: %d\n", my_rank, sum);
    MPI_Finalize();
    return 0;
}
