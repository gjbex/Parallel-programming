#include <stdio.h>

int main() {
    const int N = 1000;
    int data[N];
    const int search_value = 5;
    int count = 0;
    int found_idx = -1;
    int has_found = 0;
#   pragma omp parallel default(none) shared(data, N, search_value, count, found_idx, has_found)
    {
#       pragma omp for
        for (int i = 0; i < N; i++) {
            data[i] = 0;
        }
#       pragma omp single
        data[2] = search_value;
#       pragma omp for reduction(+: count)
        for (int i = 0; i < N; i++) {
            if (data[i] == search_value) {
                found_idx = i;
                has_found = 1;
#               pragma omp cancel for
            }
            if (i % 5 == 0) {
#               pragma omp cancellation point for
            }
            count++;
        }
    }
    if (has_found) {
        printf("found %d at %d", search_value, found_idx);
    } else {
        printf("not found");
    }
    printf(", count = %d versus %d\n", count, N);
    return 0;
}
