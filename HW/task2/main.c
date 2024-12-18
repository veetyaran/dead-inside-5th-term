#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "matrix_initialization.h"
#include "invert_matrix.h"
#include "print_matrix.h"
#include "calculate_solution_error.h"

typedef struct {
    int n;
    int p;
    int r;
    int s;
    char* filename;
} Args;

typedef struct {
	int n;
	double *a;
	double *x;
	int my_rank;
	int total_threads;
} RoutineArgs;

int parseArgs(int argc, char** argv, Args* desc);

int parseInt(const char* str, int* desc);

void* inversion(void* p_arg);

long int get_time(void);

long int get_full_time(void);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long int thread_time = 0;

int main(int argc, char** argv) {
    /* Парсим аргументы командной строки */
    Args args = {0, 0, 0, 0, NULL};
    double* a = NULL;
    double* x = NULL;
    pthread_t* threads = NULL;
    RoutineArgs* routineArgs = NULL;
    FILE* file = NULL;
    double r1 = 0.0;
    double r2 = 0.0;
    long int t1 = 0;
    clock_t t2 = 0;
    int err = 0;
    int retval = 0;
    int i = 0;

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

    threads = (pthread_t*)malloc(args.p * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "[ERROR] failed to allocate memory for threads.\n");
        perror("malloc");
        retval = 1;
        goto free_x; 
    }

    routineArgs = (RoutineArgs*)malloc(args.p * sizeof(RoutineArgs));
    if (routineArgs == NULL) {
        fprintf(stderr, "[ERROR] failed to allocate memory for routine args.\n");
        perror("malloc");
        retval = 1;
        goto free_threads; 
    }

    for (i = 0; i < args.p; ++i) {
        routineArgs[i].n = args.n;
		routineArgs[i].a = a;
		routineArgs[i].x = x;
		routineArgs[i].my_rank = i;
		routineArgs[i].total_threads = args.p;
    }

    if (args.s == 0) {
        file = fopen(args.filename, "r");
        if (file == NULL) {
            fprintf(stderr, "[ERROR] failed to open file=`%s`.\n", args.filename);
            perror("open");
            retval = 1;
            goto free_args;
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
    
    t1 = get_full_time();
    // t1 = clock();
    for (i = 0; i < args.p; ++i) {
        err = pthread_create(threads + i, 0, inversion, routineArgs + i);
        if (err != 0) {
            fprintf(stderr, "[ERROR] failed to create thread.\n");
            retval = 1;
            goto close_file;
        }
    }

    for (i = 0; i < args.p; ++i) {
        err = pthread_join(threads[i], 0);
        if (err != 0) {
            fprintf(stderr, "[ERROR] failed to join thread.\n");
            retval = 1;
            goto close_file;
        }
    }
    
    // t1 = clock() - t1;
    t1 = get_full_time() - t1;

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
        "%s: Task = %d Res1 = %e Res2 = %e T1 = %.2f T2 = %.2f S = %d N = %d P = %d\n",
        argv[0], 24, r1, r2, (double)t1/ 100.0, (double)t2 / CLOCKS_PER_SEC, args.s, args.n, args.p
    );


close_file:
    if (file != NULL) {
        fclose(file);
    }
free_args:
    free(routineArgs);
free_threads:
    free(threads);
free_x:
    free(x);
free_a:
    free(a);
    
    return retval;
}

int parseArgs(int argc, char** argv, Args* desc) {
    int err = 0;
    int n = 0;
    int p = 0;
    int r = 0;
    int s = 0;
    char* filename = NULL;

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

    err = parseInt(argv[2], &p);
    if (err != 0) {
        fprintf(stderr, "[ERROR] 'p' can't be converted to int.\n");
        return 1;
    }

    if (p < 1) {
        fprintf(stderr, "[ERROR] 'p' should be greater than 0.\n");
        return 1;
    }

    err = parseInt(argv[3], &r);
    if (err != 0) {
        fprintf(stderr, "[ERROR] 'r' can't be converted to int.\n");
        return 1;
    }

    if (r < 1) {
        fprintf(stderr, "[ERROR] 'r' should be greater than 0.\n");
        return 1;
    }

    err = parseInt(argv[4], &s);
    if (err != 0) {
        fprintf(stderr, "[ERROR] 's' can't be converted to int.\n");
        return 1;
    }

    if (s < 0 || s > 4) {
        fprintf(stderr, "[ERROR] 's' should be in range [0; 4].\n");
        return 1;
    }

    if (s == 0 && argc < 6) {
        fprintf(stderr, "[ERROR] filename is not specified.\n");
        return 1;
    }

    if (s == 0) {
        filename = argv[5];
    }

    desc->n = n;
    desc->p = p;
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

void* inversion(void* p_arg) {
    RoutineArgs* arg = (RoutineArgs*)p_arg;
	long int t1;

	t1 = get_time();
	InvertMatrix(arg->n, arg->a, arg->x, arg->my_rank, arg->total_threads);
	t1 = get_time() - t1;

	pthread_mutex_lock(&mutex);
	thread_time += t1;
	pthread_mutex_unlock(&mutex);

	return NULL;
}


long int get_time(void) {
	struct rusage buf;

	getrusage(RUSAGE_SELF, &buf);

	return buf.ru_utime.tv_sec * 100 + buf.ru_utime.tv_usec/10000;
}

long int get_full_time(void) {
	struct timeval buf;

	gettimeofday(&buf, 0);

	return buf.tv_sec * 100 + buf.tv_usec/10000;
}

