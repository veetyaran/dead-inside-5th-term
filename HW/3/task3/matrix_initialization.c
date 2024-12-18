#include "matrix_initialization.h"

#define Max(x, y) (((x) > (y)) ? (x) : (y))
#define Abs(x) (((x) < 0) ? (-(x)) : (x))

/* 
    Функция для инициализации элементов матрицы.
    Предполгается, что k in [1; 4].

    К индексам везде добавляется 1, т.к. в программе 0-индексация, 
    а задании требуется 1-индексация. 
*/
double f(int k, int n, int i, int j) {
    switch (k) {
    case 1:
        return n - Max(i, j);
    case 4:
        return 1.0 / (i + j + 1.0); 
    case 2:
        if (i == j) {
            return 2.0;
        }

        if (Abs(i - j) == 1) {
            return -1.0;
        }

        return 0.0;
    case 3:
        if (j == n - 1) {
            return i + 1.0;
        }

        if (i == n - 1) {
            return j + 1.0;
        }

        if (i == j) {
            return 1.0;
        }

        return 0.0;
    }

    return 0.0;
}

void InitMatrixByFunction(
    double* a, 
    int n, 
    int s,
    double (*func)(int, int, int, int)
) {
    int i = 0;
    int j = 0;

    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            a[i*n + j] = func(s, n, i, j);
        }
    }
}

int InitMatrixFromFile(
    double* a,
    int n, 
    FILE* file
) {
    double tmp = 0;
    int i = 0;
    int j = 0;

    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            switch (fscanf(file, "%lf", &tmp)) {
                case EOF:
                    fprintf(stderr, "[ERROR] unexpected end of file.\n");
                    return 1;
                case 0:
                    fprintf(stderr, "[ERROR] data in file has invalid format.\n");
                    return 1;
            }

            a[i*n + j] = tmp;
        } 
    }

    return 0;
}

