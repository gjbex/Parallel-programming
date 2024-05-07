#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int n = 10000000;
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        double x = (i + 0.5)/n;
        sum += 4.0/(1.0 + x*x);
    }
    double pi = sum/n;
    printf("pi = %.15f\n", pi);
    return 0;
}
