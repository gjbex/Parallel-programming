#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    static int i = -17;
    int j = -13;
    static int *k;
    printf("threadprivate(i, k) private(j) first parallel region:\n");
#pragma omp threadprivate(i, k) 
#pragma omp parallel default(none) private(j) copyin(i)
    {
        k = (int *) malloc(sizeof(int));
        j = omp_get_thread_num();
        *k = omp_get_thread_num();
        printf("  %4d -> %d, %d, %d\n", omp_get_thread_num(), i, j, *k);
    }
    printf("threadprivate(i) second parallel region:\n");
#pragma omp parallel default(none) private(j)
    {
        printf("  %4d -> %d, %d, %d\n", omp_get_thread_num(), i, j, *k);
        free(k);
    }
    return 0;
}
