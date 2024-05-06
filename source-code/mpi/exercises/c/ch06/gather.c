#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main() {
    MPI_Init(NULL, NULL);
    int my_rank, nr_procs;
    const int root = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procs);
    int *recv_buffer;
    if (my_rank == root) {
        if ((recv_buffer = (int *) malloc(nr_procs*sizeof(int))) == NULL) {
            fprintf(stderr, "malloc failed\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    int send_buffer = my_rank;
    MPI_Gather(&send_buffer, 1, MPI_INT, recv_buffer, 1, MPI_INT, root, MPI_COMM_WORLD);
    if (my_rank == root) {
        for (int i = 0; i < nr_procs; i++) {
            printf("%d\n", recv_buffer[i]);
        }
        free(recv_buffer);
    }
    MPI_Finalize();
    return 0;
}
