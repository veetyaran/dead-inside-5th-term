#include <stdio.h>

#include "print_matrix.h"

void PrintMatrix(double* a, int n, int m, int r) {
    int i = 0;
    int j = 0;

    for (i = 0; i < n && i < r; ++i) {
        for (j = 0; j < m && j < r; ++j) {
            printf(" %10.3e", a[i*n + j]);
        }
        printf("\n");
    }
}
