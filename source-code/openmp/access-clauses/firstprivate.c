#include <stdio.h>
#include <omp.h>

int main() {
    int i = -17;
    printf("private(i):\n");
#pragma omp parallel default(none) private(i)
    printf("  %4d -> %d\n", omp_get_thread_num(), i);
    printf("firstprivate(i):\n");
#pragma omp parallel default(none) firstprivate(i)
    printf("  %4d -> %d\n", omp_get_thread_num(), i);
    return 0;
}
