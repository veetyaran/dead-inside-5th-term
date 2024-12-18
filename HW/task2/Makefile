FLAGS = -DM=10 -O3 -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual
all: main.o calculate_solution_error.o invert_matrix.o matrix_initialization.o print_matrix.o
	gcc $(FLAGS) main.o calculate_solution_error.o invert_matrix.o matrix_initialization.o print_matrix.o -lm -lpthread -o prog -O2

main.o: main.c calculate_solution_error.h invert_matrix.h print_matrix.h
	gcc $(FLAGS) main.c -c -o main.o 

calculate_solution_error: calculate_solution_error.c calculate_solution_error.h
	gcc $(FLAGS) calculate_solution_error.c -c -o calculate_solution_error.o 

invert_matrix: invert_matrix.c invert_matrix.h
	gcc $(FLAGS) invert_matrix.c -c -o invert_matrix.o 

matrix_initialization: matrix_initialization.c matrix_initialization.h
	gcc $(FLAGS) matrix_initialization.c -c -o matrix_initialization.o 

print_matrix.o: print_matrix.c print_matrix.h
	gcc $(FLAGS) print_matrix.c -c -o print_matrix.o 

clean:
	rm -f *.o prog
