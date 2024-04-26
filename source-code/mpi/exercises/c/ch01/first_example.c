#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    /* MPI-related variables */
    int my_rank, nr_procs;
    const int root = 0;
    const int tag = 17;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nr_procs);
    /* application-related data */
    int n;
    double result;
    if (my_rank == 0) {
        printf("Enter the number of elements (n): ");
        fflush(stdout);
        scanf("%d", &n);
    }
    /* send n to all processes */
    MPI_Bcast(&n, 1, MPI_INT, root, MPI_COMM_WORLD);
    /* compute the result */
    result = (double) my_rank*n;
    printf("I am process %d out of %d handling the %dth part of n = %d elements, result = %lf\n",
            my_rank, nr_procs, my_rank, n, result);
    if (my_rank != root) {
        /* send the result to the root process */
        MPI_Send(&result, 1, MPI_DOUBLE, root, tag, MPI_COMM_WORLD);
    } else {
        printf("I am process 0; my own result is %lf\n", result);
        /* receive the results from the other processes */
        for (int rank = 1; rank < nr_procs; rank++) {
            MPI_Recv(&result, 1, MPI_DOUBLE, rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("I am process 0; received the result %lf from process %d\n",
                    result, rank);
        }
    }
    MPI_Finalize();
    return 0;
}
