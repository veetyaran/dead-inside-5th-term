#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matrix_initialization.h"
#include "invert_matrix.h"
#include "print_matrix.h"
#include "calculate_solution_error.h"

typedef struct {
    int n;
    int r;
    int s;
    char* filename;
} Args;

int parseArgs(int argc, char** argv, Args* desc);

int parseInt(const char* str, int* desc);

int main(int argc, char** argv) {
    /* Парсим аргументы командной строки */
    Args args = {0, 0, 0, NULL};
    double* a = NULL;
    double* x = NULL;
    FILE* file = NULL;
    double r1 = 0.0;
    double r2 = 0.0;
    clock_t t1 = 0;
    clock_t t2 = 0;
    int err = 0;
    int retval = 0;

    err = parseArgs(argc, argv, &args);
    if (err != 0) {
        fprintf(stderr, "[ERROR] got invalid command line arguments.\n");
        return 1; 
    }

    /* Инициализируем матрицу и вектор */
    a = (double*)malloc(args.n * args.n * sizeof(double));
    if (a == NULL) {
        fprintf(stderr, "[ERROR] failed to allocate memory for matrix.\n");
        perror("malloc");
        return 1; 
    }

    x = (double*)malloc(args.n * args.n * sizeof(double));
    if (x == NULL) {
        fprintf(stderr, "[ERROR] failed to allocate memory for vector.\n");
        perror("malloc");
        retval = 1;
        goto free_a;
    }

    if (args.s == 0) {
        file = fopen(args.filename, "r");
        if (file == NULL) {
            fprintf(stderr, "[ERROR] failed to open file=`%s`.\n", args.filename);
            perror("open");
            retval = 1;
            goto free_x;
        }

        err = InitMatrixFromFile(a, args.n, file);
        if (err != 0) {
            fprintf(stderr, "[ERROR] failed to init matrix from file.\n");
            retval = 1;
            goto close_file;
        }
    } else {
        InitMatrixByFunction(a, args.n, args.s, f);
    }

    /* Печатаем исходную матрицу */
    PrintMatrix(a, args.n, args.n, args.r);
    printf("\v");

    /* Обращаем матрицу и фиксируем время работы */
    t1 = clock();
    err = InvertMatrix(args.n, a, x);
    t1 = clock() - t1;

    if (args.s == 0) {
        rewind(file);

        err = InitMatrixFromFile(a, args.n, file);
        if (err != 0) {
            fprintf(stderr, "[ERROR] failed to init matrix from file.\n");
            retval = 1;
            goto close_file;
        }
    } else {
        InitMatrixByFunction(a, args.n, args.s, f); 
    }

    /* Если метод применим к матрице */
    if (err == 0) {
        /* Печатаем обратную матрицу */
        PrintMatrix(x, args.n, args.n, args.r);
        printf("\v");

        /* Вычисляем значения невязки */
        t2 = clock();
        CalculateSolutionError(a, x, args.n, &r1, &r2);
        t2 = clock() - t2;
    /* Если метод неприменим, выставляем дефолтные значения */
    } else {
        r1 = -1.0;
        r2 = -1.0;
        t2 = 0; 
    }

    printf(
        "%s: Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d N = %d\n",
        argv[0], 24, r1, r2, (double)t1/ CLOCKS_PER_SEC, (double)t2 / CLOCKS_PER_SEC, args.s, args.n 
    );


close_file:
    if (file != NULL) {
        fclose(file);
    }
free_x:
    free(x);
free_a:
    free(a);
    
    return retval;
}

int parseArgs(int argc, char** argv, Args* desc) {
    int err = 0;
    int n = 0;
    int r = 0;
    int s = 0;
    char* filename = NULL;

    if (argc < 4) {
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

    err = parseInt(argv[2], &r);
    if (err != 0) {
        fprintf(stderr, "[ERROR] 'r' can't be converted to int.\n");
        return 1;
    }

    if (r < 1) {
        fprintf(stderr, "[ERROR] 'r' should be greater than 0.\n");
        return 1;
    }

    err = parseInt(argv[3], &s);
    if (err != 0) {
        fprintf(stderr, "[ERROR] 's' can't be converted to int.\n");
        return 1;
    }

    if (s < 0 || s > 4) {
        fprintf(stderr, "[ERROR] 's' should be in range [0; 4].\n");
        return 1;
    }

    if (s == 0 && argc < 5) {
        fprintf(stderr, "[ERROR] filename is not specified.\n");
        return 1;
    }

    if (s == 0) {
        filename = argv[4];
    }

    desc->n = n;
    desc->r = r;
    desc->s = s;
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

