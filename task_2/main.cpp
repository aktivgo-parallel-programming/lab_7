//
// Created by vladislav on 19.05.22.
//
#include <iostream>
#include <mpich/mpi.h>
#include <cstdlib>
#include <cmath>
#include <vector>

std::vector<int> create_vector(int);

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int proc_num;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    int n = 0;
    std::vector<int> counts;
    std::vector<int> offsets;
    std::vector<int> vector;

    if (proc_rank == 0) {
        std::cout << "Enter n: ";
        std::cin >> n;

        if (n < 1 || n > 5 * 1e8) {
            printf("wrong n");
            return -1;
        }

        vector = create_vector(n);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < proc_num; ++i) {
        int partSize = proc_rank != proc_num - 1 ? n / (proc_num - 1) : n / (proc_num - 1) + n % (proc_num - 1);
        int k = proc_rank == 0 ? 1 : proc_rank * partSize + 1;
        counts.push_back(partSize);
        offsets.push_back(k);
    }

    std::vector<int> buff(100);
    MPI_Scatterv(vector.data(), counts.data(), offsets.data(), MPI_INT,
                 &buff, counts[proc_rank], MPI_INT, 0, MPI_COMM_WORLD);

    bool isSorted = true;
    for (int i = 0; i < vector.size() - 1; ++i) {
        if (vector[i] > vector[i + 1]) {
            isSorted = false;
            break;
        }
    }

    bool resSorted = false;
    MPI_Reduce(&isSorted,&resSorted, 1, MPI_C_BOOL, MPI_LAND, 0, MPI_COMM_WORLD);

    if (proc_rank == 0) {
        printf("res sorted = %d\n", resSorted);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}

std::vector<int> create_vector(int n) {
    std::vector<int> vector;

    for (int i = 0; i < n; ++i) {
        vector.push_back(i);
    }

    return vector;
}