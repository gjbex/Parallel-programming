#include <stdio.h>
#include <mpi.h>

int main() {
    MPI_Init(NULL, NULL);
    int my_rank, nr_procs;
    const int root = 0;
    const int TAG = 17;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procs);
    if (my_rank == root) {
        printf("%d\n", my_rank);
        for (int i = 1; i < nr_procs; i++) {
                int recv_buffer;
                MPI_Recv(&recv_buffer, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("%d\n", recv_buffer);
        }
    } else {
        int send_buffer = my_rank;
        MPI_Send(&send_buffer, 1, MPI_INT, root, TAG, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}
