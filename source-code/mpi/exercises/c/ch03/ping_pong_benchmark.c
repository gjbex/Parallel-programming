#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    MPI_Init(NULL, NULL);
    int my_rank, nr_procs;
    const int TAG = 17;
    const int root = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procs);
    int nr_ping_pongs, message_size;
    if (my_rank == root) {
        if (nr_procs < 2) {
            fprintf(stderr, "must be run with at least two processes\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        nr_ping_pongs = argc > 1 ? atoi(argv[1]) : 10;
        message_size = argc > 2 ? atoi(argv[2]) : 1;
    }
    MPI_Bcast(&nr_ping_pongs, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Bcast(&message_size, 1, MPI_INT, root, MPI_COMM_WORLD);
    // create buffer
    char *buffer;
    if ((buffer = (char *) malloc(message_size)) ==NULL) {
        fprintf(stderr, "buffer can not be created\n");
        MPI_Abort(MPI_COMM_WORLD, 2);
    }
    for (int i = 0; i < message_size; i++) {
        buffer[i] = '0' + (i % 10);
    }
    int source, dest;
    // warm up
    if (my_rank == 0) {
        source = dest = 1;
        MPI_Send(buffer, message_size, MPI_CHAR, dest, TAG, MPI_COMM_WORLD);
        MPI_Recv(buffer, message_size, MPI_CHAR, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else if (my_rank == 1) {
        source = dest = 0;
        MPI_Recv(buffer, message_size, MPI_CHAR, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(buffer, message_size, MPI_CHAR, dest, TAG, MPI_COMM_WORLD);
    }
    double start_time = MPI_Wtime();
    for (int i = 0; i < nr_ping_pongs; i++) {
        if (my_rank == 0) {
            source = dest = 1;
            MPI_Send(buffer, message_size, MPI_CHAR, dest, TAG, MPI_COMM_WORLD);
            MPI_Recv(buffer, message_size, MPI_CHAR, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else if (my_rank == 1) {
            source = dest = 0;
            MPI_Recv(buffer, message_size, MPI_CHAR, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(buffer, message_size, MPI_CHAR, dest, TAG, MPI_COMM_WORLD);
        }
    }
    double end_time = MPI_Wtime();
    if (my_rank == root) {
        printf("time for 1 message of size %d bytes: %lf\n",
                message_size, 0.5*(end_time - start_time)/nr_ping_pongs);
    }
    MPI_Finalize();
    return 0;
}
