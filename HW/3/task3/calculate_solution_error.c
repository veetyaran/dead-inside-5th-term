#include <float.h>
#include <math.h>

#include "calculate_solution_error.h"

#define Max(x, y) (((x) > (y)) ? (x) : (y))
#define Abs(x) (((x) < 0) ? (-(x)) : (x))

void CalculateSolutionError(
    const double* a, 
    const double* values, 
    int n, 
    double* r1, 
    double* r2
) {
    double tmp1 = 0.0;
    double tmp2 = 0.0;
    double matrixNorm = 0.0;
    int i = 0;
    int j = 0;

    /* Вычисляем матричную норму */
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            matrixNorm += Abs(a[i*n + j]);
        }
    }

    /* Вычисляем норму в первом инварианте */
    for (i = 0; i < n; ++i) {
        tmp1 += a[i*n + i] - values[i];
    }

    *r1 = Abs(tmp1) / matrixNorm;

    /* Вычисляем норму во втором инварианте */
    tmp1 = 0.0;
    for (i = 0; i < n; ++i) {
        tmp1 += values[i] * values[i];
    }
    
    tmp1 = sqrt(tmp1);

    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            tmp2 += a[i*n + j] * a[j*n + i];
        }
    }

    tmp2 = sqrt(tmp2);

    *r2 = Abs(tmp2 - tmp1) / matrixNorm; 
}