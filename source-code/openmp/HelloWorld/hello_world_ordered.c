#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main() {
    int thread_num = 0, num_threads = 1;
#pragma omp parallel default(none) private(thread_num) shared(num_threads)
    {
#ifdef _OPENMP
        thread_num = omp_get_thread_num();
        num_threads = omp_get_num_threads();
#endif
#pragma omp for ordered
        for (int i = 0; i < num_threads; i++) {
#pragma omp ordered
            printf("hello from thread number %d\n", thread_num);
        }
    }
    printf("%d threads total\n", num_threads);
    return 0;
}
