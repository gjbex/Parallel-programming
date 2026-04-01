#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

void print_system(const float *temp, const int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%12.6f", temp[i*n + j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    // dimension of the grid
    int n = 10;
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    // maximum number of time steps
    int t_max = 5;
    if (argc > 2) {
        t_max = atoi(argv[2]);
    }
    // delta value to stop
    const float diff_stop = 1e-3f;

    // allocate temperature matrices
    float *temp = (float *) malloc(n*n*sizeof(float));
    float *prev_temp = (float *) malloc(n*n*sizeof(float));
    if (temp == NULL || prev_temp == NULL) {
        fprintf(stderr, "error: can not allocat %dx%d array\n", n, n);
        return 1;
    }

    // initialize temperatures
#pragma omp parallel default(none) shared(temp, prev_temp, n)
    {
#pragma omp for collapse(2)
        for (int i = 1; i < n; i++) {
            for (int j = 1; j < n - 1; j++) {
                prev_temp[i*n + j] = temp[i*n + j] = 0.0f;
            }
        }
#pragma omp for
        for (int j = 0; j < n; j++) {
            prev_temp[j] = temp[j] = 1.0f;
        }
#pragma omp for
        for (int i = 0; i < n; i++) {
            prev_temp[i*n] = prev_temp[(i + 1)*n - 1] = temp[i*n] = temp[(i + 1)*n - 1] = ((float) (n - i - 1))/(n - 1);
        }
    }

    // do time steps
    float max_diff = 0.0f;
    int is_done = FALSE;
    int max_t = 0;
#pragma omp parallel default(none) \
    shared(temp, prev_temp, n, t_max, max_diff, diff_stop, is_done, max_t)
    for (int t = 0; t < t_max; t++) {
#pragma omp single
        {
            max_diff = 0.0f;
        }

#pragma omp barrier
        if (is_done) {
            continue;
        }

#pragma omp for collapse(2) reduction(max : max_diff)
        for (int i = 1; i < n - 1; i++) {
            for (int j = 1; j < n - 1; j++) {
                temp[i*n + j] = 0.25f * (prev_temp[(i - 1)*n + j]
                        + prev_temp[(i + 1)*n + j]
                        + prev_temp[i*n + j - 1]
                        + prev_temp[i*n + j + 1]);
                float diff = fabsf(temp[i*n + j] - prev_temp[i*n + j]);
                if (diff > max_diff) {
                    max_diff = diff;
                }
            }
        }

#pragma omp single
        {
            max_t = t + 1;
            if (max_diff < diff_stop) {
                is_done = TRUE;
            }
            float *tmp = temp;
            temp = prev_temp;
            prev_temp = tmp;
        }

#pragma omp barrier
    }
    print_system(prev_temp, n);
    printf("%d steps: %f\n", max_t, max_diff);

    // deallocate matrices
    free(temp);
    free(prev_temp);

    return 0;
}
