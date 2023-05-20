#include <float.h>
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
    const float diff_stop = 1e-5f;

    // allocate temperature matrices
    float *temp = (float *) malloc(n*n*sizeof(float));
    float *prev_temp = (float *) malloc(n*n*sizeof(float));
    if (temp == NULL || prev_temp == NULL) {
        fprintf(stderr, "error: can not allocat %dx%d array\n", n, n);
        return 1;
    }

    // initialize temperatures
#pragma omp parallel for default(none) shared(temp, prev_temp, n)
    for (int i = 0; i < n*n; i++) {
        prev_temp[i] = temp[i] = 0.0f;
    }
    for (int i = 0; i < n; i++) {
        prev_temp[i] = temp[i] = 1.0f;
    }

    // do time steps
    float max_diff = -FLT_MAX;
    int t;
    int is_done = FALSE;
#pragma omp parallel default(none) shared(temp, prev_temp, n, t_max, max_diff, diff_stop, is_done, stderr) private(t)
    for (t = 0; t < t_max && !is_done; t++) {
        max_diff = -FLT_MAX;
        float local_diff = -FLT_MAX;
#pragma omp for reduction(max:max_diff) collapse(2)
        for (int i = 1; i < n - 1; i++) {
            for (int j = 1; j < n - 1; j++) {
                temp[i*n + j] = 0.25*(prev_temp[(i - 1)*n + j] + prev_temp[(i + 1)*n + j] +
                        prev_temp[i*n + j - 1] + prev_temp[i*n + j + 1]);
                float diff = fabs(temp[i*n + j] - prev_temp[i*n + j]);
                if (diff > local_diff) {
                    local_diff = diff;
                }
            }
        }
#pragma omp critical
        if (local_diff > max_diff) {
            max_diff = local_diff;
        }
#pragma omp single
        {
            fprintf(stderr, "step %d: %f\n", t, max_diff);
            float *tmp = temp;
            temp = prev_temp;
            prev_temp = tmp;
            if (max_diff < diff_stop) {
                is_done = TRUE;
            }
        }
#pragma omp barrier
    }
    print_system(temp, n);

    // deallocate matrices
    free(temp);
    free(prev_temp);

    return 0;
}
