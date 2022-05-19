//
// Created by vladislav on 19.05.22.
//
#include <iostream>
#include <mpich/mpi.h>
#include <cstdlib>
#include <cmath>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int proc_num;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    int n = 0;
    if (proc_rank == 0) {
        std::cout << "Enter n: ";
        std::cin >> n;

        if (n < 1 || n > 5 * 1e9) {
            printf("wrong n");
            return -1;
        }
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int countSums = proc_rank != proc_num - 1 ? n / (proc_num - 1) : n / (proc_num - 1) + n % (proc_num - 1);
    int k = proc_rank == 0 ? 1 : proc_rank * countSums + 1;

    double sum = 0;
    for (int i = k; i < k + countSums; ++i) {
        sum += pow(-1, i - 1) / i;
    }

    double resSum = 0;
    MPI_Reduce(&sum,&resSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (proc_rank == 0) {
        printf("res sum = %f\n", resSum);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}