#include <mpi.h>
#include <stdio.h>

int main() {
    MPI_Init(NULL, NULL);
    printf("Hello, World!\n");
    MPI_Finalize();
    return 0;
}
