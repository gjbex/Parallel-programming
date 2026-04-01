#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void print_system(const float *temp, int n);

static int run_simulation(const int n,
                          const int t_max,
                          const int print_solution) {
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
    // top edge
    for (int j = 0; j < n; j++) {
        prev_temp[j] = temp[j] = 1.0f;
    }
    // inner points, including bottom edge
    for (int i = 1; i < n; i++) {
        for (int j = 1; j < n - 1; j++) {
            prev_temp[i*n + j] = temp[i*n + j] = 0.0f;
        }
    }
    // left and right edges
    for (int i = 0; i < n; i++) {
        prev_temp[i*n] = prev_temp[(i + 1)*n - 1] = temp[i*n] = temp[(i + 1)*n - 1] = ((float) (n - i - 1))/(n - 1);
    }

    // do time steps
    int max_t = 0;
    float max_diff = 0.0f;
    for (int t = 0; t < t_max; t++) {
        max_diff = -FLT_MAX;
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
        max_t = t + 1;
        fprintf(stderr, "step %d: %12.6f\n", t + 1, max_diff);
        float *tmp = temp;
        temp = prev_temp;
        prev_temp = tmp;
        if (max_diff < diff_stop) {
            break;
        }
    }
    if (print_solution) {
        print_system(prev_temp, n);
    }
    printf("%d steps: %f\n", max_t, max_diff);

    // deallocate matrices
    free(temp);
    free(prev_temp);

    return 0;
}

static void print_system(const float *temp, const int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%12.6f", temp[i*n + j]);
        }
        printf("\n");
    }
}

enum {
    DEFAULT_GRID_SIZE = 10,
    DEFAULT_MAX_STEPS = 5,
    MIN_GRID_SIZE = 2,
    MIN_MAX_STEPS = 1
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
                                        int *print_solution);

int main(int argc, char *argv[]) {
    // dimension of the grid
    int n = DEFAULT_GRID_SIZE;
    // maximum number of time steps
    int t_max = DEFAULT_MAX_STEPS;
    int print_solution = 0;

    const int parse_status = parse_command_line_arguments(argc,
                                                          argv,
                                                          &n,
                                                          &t_max,
                                                          &print_solution);
    if (parse_status != 2) {
        return parse_status;
    }

    return run_simulation(n, t_max, print_solution);
}

static void print_usage(const char *program) {
    fprintf(stderr,
            "Usage: %s [-n grid_size] [-t max_steps] [-s]\n"
            "  -n grid_size  Grid dimension, integer >= %d\n"
            "  -t max_steps  Maximum number of time steps, integer >= %d\n"
            "  -s            Print the final temperature matrix\n"
            "  -h            Show this help message\n",
            program,
            MIN_GRID_SIZE,
            MIN_MAX_STEPS);
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
                                        int *print_solution) {
    int opt = 0;

    while ((opt = getopt(argc, argv, "hn:t:s")) != -1) {
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
