#include <stdio.h>
#include <mpi.h>

int main() {
    MPI_Init(NULL, NULL);
    int my_rank, nr_procs;
    const int TAG = 17;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procs);
    if (nr_procs < 2 && my_rank == 0) {
        fprintf(stderr, "must be run with at least two processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    if (my_rank == 0) {
        int send_buffer = my_rank;
        int dest = 1;
        printf("rank %d sending %d to rank %d\n", 0, send_buffer, dest);
        MPI_Send(&send_buffer, 1, MPI_INT, dest, TAG, MPI_COMM_WORLD);
    } else if (my_rank == 1) {
        int recv_buffer;
        int source = 0;
        MPI_Status status;
        MPI_Recv(&recv_buffer, 1, MPI_INT, source, TAG, MPI_COMM_WORLD, &status);
        printf("rank %d received %d from rank %d\n", 1, recv_buffer, status.MPI_SOURCE);
    }
    MPI_Finalize();
    return 0;
}
