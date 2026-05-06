#include <stdio.h>
#include <mpi.h>

int main() {
    const int TAG = 17;
    MPI_Init(NULL, NULL);
    int my_rank, nr_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procs);
    int send_buffer = my_rank;
    int dest = (my_rank + 1) % nr_procs;
    int source = (my_rank - 1 + nr_procs) % nr_procs;
    int sum = 0;
    for (int i = 0; i < nr_procs; i++) {
        MPI_Request request;
        MPI_Issend(&send_buffer, 1, MPI_INT, dest, TAG, MPI_COMM_WORLD, &request);
        int recv_buffer;
        MPI_Recv(&recv_buffer, 1, MPI_INT, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        sum += recv_buffer;
        MPI_Wait(&request, MPI_STATUS_IGNORE);
        send_buffer = recv_buffer;
    }
    printf("rrank %d: %d\n", my_rank, sum);
    MPI_Finalize();
    return 0;
}
