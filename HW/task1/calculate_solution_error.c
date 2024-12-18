#include <float.h>

#include "calculate_solution_error.h"

#define Max(x, y) (((x) > (y)) ? (x) : (y))
#define Abs(x) (((x) < 0) ? (-(x)) : (x))

void CalculateSolutionError(
    const double* a, 
    const double* x, 
    int n, 
    double* r1, 
    double* r2
) {
    double sum1 = 0.0;
    double sum2= 0.0;
    double y1 = 0.0;
    double y2 = 0.0;
    double max_sum1 = DBL_MIN;
    double max_sum2 = DBL_MIN;
    int i = 0;
    int j = 0;
    int k = 0;

    if (n > 11000) {
        *r1 = 0.0;
        *r2 = 0.0;
        
        return;
    }


    /* Идем по столбцам результирующей матрицы и собираем сумму модулей */
    for (j = 0; j < n; ++j) {
        /* Суммы модулей элементов результирующего столбца */
        sum1 = 0.0;
        sum2 = 0.0;

        /* Идем по строкам j-ого столбца результирующей матрицы */
        for (i = 0; i < n; ++i) {
            /* Значения элементов результирующей матрицы на местах (i, j) */
            y1 = 0.0;
            y2 = 0.0;

            /* Вычисляем элемент произведения */
            for (k = 0; k < n; ++k) {
                y1 += a[i*n + k] * x[k*n + j];
                y2 += x[i*n + k] * a[k*n + j];
            }
            
            /* Учитываем необходимость отнять единичную матрицу */
            if (i == j) {
                y1 -= 1.0;
                y2 -= 1.0;
            }

            sum1 += Abs(y1);
            sum2 += Abs(y2);
        }

        max_sum1 = Max(max_sum1, sum1);
        max_sum2 = Max(max_sum2, sum2);
    }

    *r1 = max_sum1;
    *r2 = max_sum2;
}
