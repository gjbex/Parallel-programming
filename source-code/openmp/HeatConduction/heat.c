#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void print_system(const float *temp, int n);
static double compute_checksum(const float *temp, int n);
static int min_int(int a, int b);

static int run_simulation(const int n,
                          const int t_max,
                          const int print_solution,
                          const int i_block_size,
                          const int j_block_size) {
    float *temp = (float *) malloc(n * n * sizeof(float));
    float *prev_temp = (float *) malloc(n * n * sizeof(float));

    if (temp == NULL || prev_temp == NULL) {
        fprintf(stderr, "error: can not allocat %dx%d array\n", n, n);
        free(temp);
        free(prev_temp);
        return 1;
    }

#pragma omp parallel default(none) shared(temp, prev_temp, n)
    {
#pragma omp for schedule(static)
        for (int i = 1; i < n; i++) {
            for (int j = 1; j < n - 1; j++) {
                int k = i*n + j;
                prev_temp[k] = temp[k] = 0.0f;
            }
        }
#pragma omp for schedule(static)
        for (int j = 0; j < n; j++) {
            prev_temp[j] = temp[j] = 1.0f;
        }
#pragma omp for schedule(static)
        for (int i = 0; i < n; i++) {
            prev_temp[i * n] = prev_temp[(i + 1) * n - 1] = temp[i * n] =
                temp[(i + 1) * n - 1] = ((float) (n - i - 1)) / (n - 1);
        }
    }

#pragma omp parallel default(none) \
    shared(temp, prev_temp, n, t_max, i_block_size, j_block_size)
    for (int t = 0; t < t_max; t++) {

        if (t % 2 == 0) {
#pragma omp for schedule(static)
            for (int ii = 1; ii < n - 1; ii += i_block_size) {
                for (int jj = 1; jj < n - 1; jj += j_block_size) {
                    const int i_end = min_int(ii + i_block_size, n - 1);
                    const int j_end = min_int(jj + j_block_size, n - 1);
                    for (int i = ii; i < i_end; i++) {
                        for (int j = jj; j < j_end; j++) {
                            int k = i*n + j;
                            temp[k] = 0.25f * (prev_temp[k - n]
                                    + prev_temp[k + n]
                                    + prev_temp[k - 1]
                                    + prev_temp[k + 1]);
                        }
                    }
                }
            }
        } else {
#pragma omp for schedule(static)
            for (int ii = 1; ii < n - 1; ii += i_block_size) {
                for (int jj = 1; jj < n - 1; jj += j_block_size) {
                    const int i_end = min_int(ii + i_block_size, n - 1);
                    const int j_end = min_int(jj + j_block_size, n - 1);
                    for (int i = ii; i < i_end; i++) {
                        for (int j = jj; j < j_end; j++) {
                            int k = i*n + j;
                            prev_temp[k] = 0.25f * (temp[k - n]
                                    + temp[k + n]
                                    + temp[k - 1]
                                    + temp[k + 1]);
                        }
                    }
                }
            }
        }
    }

    if (print_solution) {
        print_system(prev_temp, n);
    }
    printf("checksum: %.9e\n", compute_checksum(prev_temp, n));

    free(temp);
    free(prev_temp);

    return 0;
}

static double compute_checksum(const float *temp, const int n) {
    double checksum = 0.0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            checksum += temp[i * n + j];
        }
    }

    return checksum;
}

static int min_int(const int a, const int b) {
    if (a < b) {
        return a;
    }
    return b;
}

static void print_system(const float *temp, const int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%12.6f", temp[i * n + j]);
        }
        printf("\n");
    }
}

enum {
    DEFAULT_GRID_SIZE = 10,
    DEFAULT_MAX_STEPS = 5,
    DEFAULT_I_BLOCK_SIZE = 16,
    DEFAULT_J_BLOCK_SIZE = 256,
    MIN_GRID_SIZE = 2,
    MIN_MAX_STEPS = 1,
    MIN_BLOCK_SIZE = 1
};

static void print_usage(const char *program);
static int parse_positive_int(const char *value,
                              const char *option_name,
                              int min_value,
                              int *result);
static int parse_command_line_arguments(int argc,
                                        char *argv[],
                                        int *n,
                                        int *t_max,
                                        int *print_solution,
                                        int *i_block_size,
                                        int *j_block_size);

int main(int argc, char *argv[]) {
    int n = DEFAULT_GRID_SIZE;
    int t_max = DEFAULT_MAX_STEPS;
    int print_solution = 0;
    int i_block_size = DEFAULT_I_BLOCK_SIZE;
    int j_block_size = DEFAULT_J_BLOCK_SIZE;

    const int parse_status = parse_command_line_arguments(argc,
                                                          argv,
                                                          &n,
                                                          &t_max,
                                                          &print_solution,
                                                          &i_block_size,
                                                          &j_block_size);
    if (parse_status != 2) {
        return parse_status;
    }

    return run_simulation(n,
                          t_max,
                          print_solution,
                          i_block_size,
                          j_block_size);
}

static void print_usage(const char *program) {
    fprintf(stderr,
            "Usage: %s [-n grid_size] [-t max_steps] [-i i_block] "
            "[-j j_block] [-s]\n"
            "  -n grid_size  Grid dimension, integer >= %d\n"
            "  -t max_steps  Maximum number of time steps, integer >= %d\n"
            "  -i i_block    Tile size in i direction, integer >= %d\n"
            "  -j j_block    Tile size in j direction, integer >= %d\n"
            "  -s            Print the final temperature matrix\n"
            "  -h            Show this help message\n",
            program,
            MIN_GRID_SIZE,
            MIN_MAX_STEPS,
            MIN_BLOCK_SIZE,
            MIN_BLOCK_SIZE);
}

static int parse_positive_int(const char *value,
                              const char *option_name,
                              const int min_value,
                              int *result) {
    char *end_ptr = NULL;

    errno = 0;
    const long parsed_value = strtol(value, &end_ptr, 10);
    if (errno != 0 || end_ptr == value || *end_ptr != '\0') {
        fprintf(stderr, "error: invalid %s '%s'\n", option_name, value);
        return 0;
    }
    if (parsed_value < min_value || parsed_value > INT_MAX) {
        fprintf(stderr,
                "error: %s must be in the range [%d, %d]\n",
                option_name,
                min_value,
                INT_MAX);
        return 0;
    }

    *result = (int) parsed_value;
    return 1;
}

static int parse_command_line_arguments(int argc,
                                        char *argv[],
                                        int *n,
                                        int *t_max,
                                        int *print_solution,
                                        int *i_block_size,
                                        int *j_block_size) {
    int opt = 0;

    while ((opt = getopt(argc, argv, "hn:t:i:j:s")) != -1) {
        switch (opt) {
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'n':
                if (!parse_positive_int(optarg,
                                        "grid size",
                                        MIN_GRID_SIZE,
                                        n)) {
                    print_usage(argv[0]);
                    return 1;
                }
                break;
            case 't':
                if (!parse_positive_int(optarg,
                                        "maximum number of time steps",
                                        MIN_MAX_STEPS,
                                        t_max)) {
                    print_usage(argv[0]);
                    return 1;
                }
                break;
            case 'i':
                if (!parse_positive_int(optarg,
                                        "i block size",
                                        MIN_BLOCK_SIZE,
                                        i_block_size)) {
                    print_usage(argv[0]);
                    return 1;
                }
                break;
            case 'j':
                if (!parse_positive_int(optarg,
                                        "j block size",
                                        MIN_BLOCK_SIZE,
                                        j_block_size)) {
                    print_usage(argv[0]);
                    return 1;
                }
                break;
            case 's':
                *print_solution = 1;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    if (optind < argc) {
        fprintf(stderr, "error: unexpected positional argument '%s'\n",
                argv[optind]);
        print_usage(argv[0]);
        return 1;
    }

    return 2;
}
