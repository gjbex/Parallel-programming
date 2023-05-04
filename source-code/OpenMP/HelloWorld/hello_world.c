#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main() {
    int thread_num = 0, num_threads = 1;
#ifdef _OPENMP
#pragma omp parallel default(none) private(thread_num) shared(num_threads)
    {
        thread_num = omp_get_thread_num();
        printf("hello from thread number %d\n", thread_num);
        if (thread_num == 0) {
            num_threads = omp_get_num_threads();
        }
    }
#endif
    printf("hello from thread number %d\n", thread_num);
    printf("%d threads total\n", num_threads);
    return 0;
}
