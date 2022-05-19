//
// Created by vladislav on 19.05.22.
//
#include <iostream>
#include <mpich/mpi.h>
#include <cstdlib>
#include <cmath>
#include <vector>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    MPI_Status status;

    int proc_num;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    if (proc_rank == 0) {
        int n = 0;

        std::cout << "Enter n: ";
        std::cin >> n;

        if (n < 1 || n > 5 * 1e9) {
            printf("wrong n");
            return -1;
        }

        int countSums = n / (proc_num - 1);
        int countSumsLast = countSums + n % (proc_num - 1);
        for (int i = 1; i < proc_num; ++i) {
            int k = (i - 1) * countSums + 1;
            MPI_Send(i != proc_num - 1 ? &countSums : &countSumsLast, 1, MPI_INT, i, 0,MPI_COMM_WORLD);
            MPI_Send(&k, 1, MPI_INT, i, 0,MPI_COMM_WORLD);
        }

        std::vector<double> sums;
        for (int i = 1; i < proc_num; ++i) {
            double sum = 0;
            MPI_Recv(&sum, 1, MPI_DOUBLE, i, MPI_ANY_TAG,MPI_COMM_WORLD, &status);
            sums.push_back(sum);
        }

        double resSum = 0;
        for(double sum : sums) {
            resSum += sum;
        }
        printf("res sum = %f\n", resSum);
    } else {
        int n;
        MPI_Recv(&n, 1, MPI_INT, 0, 0,MPI_COMM_WORLD, &status);

        int k;
        MPI_Recv(&k, 1, MPI_INT, 0, 0,MPI_COMM_WORLD, &status);

        double sum = 0;
        for(int i = k; i < k + n; i++) {
            sum += pow(-1, i - 1) / i;
        }

        MPI_Send(&sum, 1, MPI_DOUBLE, 0, proc_rank,MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}