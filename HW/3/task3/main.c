#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "matrix_initialization.h"
#include "task.h"
#include "print_matrix.h"
#include "calculate_solution_error.h"

typedef struct {
    double e;
    char* filename;
    int n;
    int m;
    int k;
} Args;

int parseArgs(int argc, char** argv, Args* desc);

int parseInt(const char* str, int* desc);

int parseDouble(const char* str, double* desc);

void* inversion(void* p_arg);

int main(int argc, char** argv) {
    /* Парсим аргументы командной строки */
    Args args = {0.0, NULL, 0, 0, 0};
    double* a = NULL;
    double* x1 = NULL;
    double* x2 = NULL;
    double* values = NULL;
    FILE* file = NULL;
    double r1 = 0.0;
    double r2 = 0.0;
    clock_t t1 = 0;
    int iter = 0;
    int err = 0;
    int retval = 0;

    err = parseArgs(argc, argv, &args);
    if (err != 0) {
        fprintf(stderr, "[ERROR] got invalid command line arguments.\n");
        return 1; 
    }

    /* Инициализируем матрицу и вектора */
    a = (double*)malloc(args.n * args.n * sizeof(double));
    x1 = (double*)malloc(args.n * sizeof(double));
    x2 = (double*)malloc(args.n * sizeof(double));
    values = (double*)malloc(args.n * sizeof(double));

    if (a == NULL || x1 == NULL || x2 == NULL || values == NULL) {
        fprintf(stderr, "[ERROR] failed to allocate memory for matrix.\n");
        perror("malloc");
        retval = 1; 
        goto free_mem;
    }

    if (args.k == 0) {
        file = fopen(args.filename, "r");
        if (file == NULL) {
            fprintf(stderr, "[ERROR] failed to open file=`%s`.\n", args.filename);
            perror("open");
            retval = 1;
            goto free_mem;
        }

        err = InitMatrixFromFile(a, args.n, file);
        if (err != 0) {
            fprintf(stderr, "[ERROR] failed to init matrix from file.\n");
            retval = 1;
            goto close_file;
        }
    } else {
        InitMatrixByFunction(a, args.n, args.k, f);
    }

    /* Печатаем исходную матрицу */
    PrintMatrix(a, args.n, args.n, args.m);
    printf("\v");

    /* Вычисляем собственные значения */
    t1 = clock();
    FindValues(args.n, a, values, args.e, x1, x2, &iter);
    t1 = clock() - t1;
    
    PrintMatrix(values, 1, args.n, args.m);

    if (args.k == 0) {
        rewind(file);

        err = InitMatrixFromFile(a, args.n, file);
        if (err != 0) {
            fprintf(stderr, "[ERROR] failed to init matrix from file.\n");
            retval = 1;
            goto close_file;
        }
    } else {
        InitMatrixByFunction(a, args.n, args.k, f); 
    }

    CalculateSolutionError(a, values, args.n, &r1, &r2);

    printf("%s : Residual1 = %e Residual2 = %e Iterations = %d Iterations1 = %d Elapsed1 = %.2f Elapsed2 = %.2f\n",
        argv[0], r1, r2, iter, iter / args.n, (double)t1 / CLOCKS_PER_SEC, (double)t1 / CLOCKS_PER_SEC
    );


close_file:
    if (file != NULL) {
        fclose(file);
    }
free_mem:
    free(a);
    free(x1);
    free(x2);
    free(values);
    
    return retval;
}

int parseArgs(int argc, char** argv, Args* desc) {
    double e = 0.0;
    char* filename = NULL;
    int err = 0;
    int n = 0;
    int m = 0;
    int k = 0;

    if (argc < 5) {
        fprintf(stderr, "[ERROR] too few command line arguments.\n");
        return 1;
    }

    err = parseInt(argv[1], &n);
    if (err != 0) {
        fprintf(stderr, "[ERROR] 'n' can't be converted to int.\n");
        return 1;
    }

    if (n < 1) {
        fprintf(stderr, "[ERROR] 'n' should be greater than 0.\n");
        return 1;
    }

    err = parseInt(argv[2], &m);
    if (err != 0) {
        fprintf(stderr, "[ERROR] 'm' can't be converted to int.\n");
        return 1;
    }

    if (m < 1) {
        fprintf(stderr, "[ERROR] 'm' should be greater than 0.\n");
        return 1;
    }

    err = parseDouble(argv[3], &e);
    if (err != 0) {
        fprintf(stderr, "[ERROR] 'e' can't be converted to int.\n");
        return 1;
    }

    if (e <= 0) {
        fprintf(stderr, "[ERROR] 'e' should be greater than 0.\n");
        return 1;
    }

    err = parseInt(argv[4], &k);
    if (err != 0) {
        fprintf(stderr, "[ERROR] 'k' can't be converted to int.\n");
        return 1;
    }

    if (k < 0 || k > 4) {
        fprintf(stderr, "[ERROR] 'k' should be in range [0; 4].\n");
        return 1;
    }

    if (k == 0 && argc < 6) {
        fprintf(stderr, "[ERROR] filename is not specified.\n");
        return 1;
    }

    if (k == 0) {
        filename = argv[5];
    }

    desc->n = n;
    desc->m = m;
    desc->e = e;
    desc->k = k;
    desc->filename = filename;

    return 0;
}

int parseInt(const char* str, int* desc) {
    int res = 0;

    res = strtol(str, NULL, 10);
    switch (errno) {
        case ERANGE:
            *desc = 0;
            fprintf(stderr, "[ERROR] converted value falls out of range.\n");

            return ERANGE;
        case EINVAL:
            *desc = 0;
            fprintf(stderr, "[ERROR] str to int convertion can't be performed.\n");
            
            return EINVAL;
        default:
            *desc = res;
    }

    return 0;
}

int parseDouble(const char* str, double* desc) {
    double res = 0;

    res = strtod(str, NULL);
    switch (errno) {
        case ERANGE:
            *desc = 0;
            fprintf(stderr, "[ERROR] converted value falls out of range.\n");

            return ERANGE;
        case EINVAL:
            *desc = 0;
            fprintf(stderr, "[ERROR] str to int convertion can't be performed.\n");
            
            return EINVAL;
        default:
            *desc = res;
    }

    return 0;
}

