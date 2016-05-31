#define ABSOLUTE_INTERVAL_BEGIN 0.0
#define ABSOLUTE_INTERVAL_END 100.0
#define PRECISION 0.000001
#define POLYNOMIAL_DEGREE 10
#define TEST_CASE 5
#define NUM_TRIALS 10

#include <mpi.h>
#include <stdio.h>
#include <math.h>

double evaluate(int polynomial[], int polynomial_length, double x_value) {
    double result = 0;
    for (int i = 0; i < polynomial_length; ++i) {
        result += pow(x_value, i) * polynomial[i];
    }
    return result;
}

void print_function(int polynomial[], int polynomial_length) {
    for (int i = 0; i < polynomial_length; ++i) {
        printf("%dx^%d", polynomial[i], i);
        if (i != polynomial_length - 1) {
            printf(" + ");
        }
    }
    printf("\n");
}


int main(int argc, char **argv) {
    srand((unsigned int) time(0));
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    int num_processes;
    int id;

    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

    for (int j = 0; j < NUM_TRIALS; ++j) {
        double initial_interval_width = (ABSOLUTE_INTERVAL_END - ABSOLUTE_INTERVAL_BEGIN) / num_processes;
        double interval_width = (ABSOLUTE_INTERVAL_END - ABSOLUTE_INTERVAL_BEGIN) / num_processes;
        double delta = 100;
        double previous_area = 0;
        double new_area = 0;
        long double averageTime = 0;

        int *polynomial = new int[POLYNOMIAL_DEGREE];

        if (id == 0) {
            if (TEST_CASE == -1) {
                for (int i = 0; i < POLYNOMIAL_DEGREE; ++i) {
                    polynomial[i] = rand() % 20;
                }
            } else if (TEST_CASE == 1) {
                int localpolynomaial[POLYNOMIAL_DEGREE] = {0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
                memcpy(polynomial, localpolynomaial, POLYNOMIAL_DEGREE * sizeof(int));
            } else if (TEST_CASE == 2) {
                int localpolynomaial[POLYNOMIAL_DEGREE] = {20, 8, 4, 19, 14, 11, 11, 1, 2, 5};
                memcpy(polynomial, localpolynomaial, POLYNOMIAL_DEGREE * sizeof(int));
            } else if (TEST_CASE == 3) {
                int localpolynomaial[POLYNOMIAL_DEGREE] = {20, 20, 17, 11, 16, 13, 8, 15, 17, 16};
                memcpy(polynomial, localpolynomaial, POLYNOMIAL_DEGREE * sizeof(int));
            } else if (TEST_CASE == 4) {
                int localpolynomaial[POLYNOMIAL_DEGREE] = {15, 19, 14, 4, 17, 19, 12, 14, 9, 18};
                memcpy(polynomial, localpolynomaial, POLYNOMIAL_DEGREE * sizeof(int));
            } else if (TEST_CASE == 5) {
                int localpolynomaial[POLYNOMIAL_DEGREE] = {11, 10, 8, 13, 4, 8, 13, 17, 5, 20};
                memcpy(polynomial, localpolynomaial, POLYNOMIAL_DEGREE * sizeof(int));
            }
//            print_function(polynomial, POLYNOMIAL_DEGREE);
        }

        MPI_Bcast(polynomial, POLYNOMIAL_DEGREE, MPI_INT, 0, MPI_COMM_WORLD);



        while (isnan(delta) || delta >= PRECISION) {
            double my_interval_begin = ABSOLUTE_INTERVAL_BEGIN + id * initial_interval_width;
            double my_interval_end = ABSOLUTE_INTERVAL_BEGIN + (id + 1) * initial_interval_width;

            int interval_array_length = (int) ((my_interval_end - my_interval_begin) / interval_width);

            double my_area = 0;
            for (int i = 0; i < interval_array_length; i++) {
                double x_value = my_interval_begin + (i * interval_width);
                my_area += evaluate(polynomial, POLYNOMIAL_DEGREE, x_value) * interval_width;
            }


            MPI_Reduce(&my_area, &new_area, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            if (id == 0) {
                delta = (new_area - previous_area) / new_area;
                previous_area = new_area;
            }

            MPI_Bcast(&delta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            interval_width /= 2.0;
            MPI_Barrier(MPI_COMM_WORLD);
        }


        if (id == 0) {
            printf("Area is: %10.10f\n", new_area);
        }
    }
    long long end_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    if (id == 0) {
        printf("Time to perform %d trials on test case %d was: %lli\n", NUM_TRIALS, TEST_CASE, (end_ms - ms));
        printf("Average time in seconds: %3.10f\n", (end_ms - ms)*1.0/NUM_TRIALS/1000.0);
    }


    MPI_Finalize();
}