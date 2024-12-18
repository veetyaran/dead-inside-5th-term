all: main.o calculate_solution_error.o task.o matrix_initialization.o print_matrix.o
	gcc main.o calculate_solution_error.o task.o matrix_initialization.o print_matrix.o -lm -lpthread -o prog -O2

main.o: main.c calculate_solution_error.h task.h print_matrix.h
	gcc main.c -c -o main.o -O2

calculate_solution_error.o: calculate_solution_error.c calculate_solution_error.h
	gcc calculate_solution_error.c -c -o calculate_solution_error.o -O2

task.o: task.c task.h
	gcc task.c -c -o task.o -O2

matrix_initialization.o: matrix_initialization.c matrix_initialization.h
	gcc matrix_initialization.c -c -o matrix_initialization.o -O2

print_matrix.o: print_matrix.c print_matrix.h
	gcc print_matrix.c -c -o print_matrix.o -O2

clean:
	rm -f *.o prog
