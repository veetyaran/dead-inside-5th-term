#ifndef MATRIX_INITIALIZATION
#define MATRIX_INITIALIZATION

#include <stdio.h>

/* 
    Функция для инициализации элементов матрицы.
    Предполгается, что s in [1; 4].
 */
double f(int s, int n, int i, int j);

void InitMatrixByFunction(
    double* a, 
    int n, 
    int s,
    double (*func)(int, int, int, int)
);

int InitMatrixFromFile(
    double* a,
    int n, 
    FILE* file
);

#endif 
