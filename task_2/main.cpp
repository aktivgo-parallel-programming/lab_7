//
// Created by vladislav on 19.05.22.
//
#include <iostream>
#include <mpich/mpi.h>
#include <cstdlib>
#include <vector>

std::vector<int> create_vector(int);
void print_vector(const std::vector<int>&);

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
        print_vector(vector);

        for (int i = 1; i < proc_num - 1; ++i) {
            int offset = i * n / proc_num;
            if (vector[offset - 1] > vector[offset]) {
                printf("vector not sorted\n");
                return EXIT_SUCCESS;
            }
        }
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < proc_num; ++i) {
        int partSize = i != proc_num - 1 ? n / proc_num : n / proc_num + n % proc_num;
        int offset = i * (n / proc_num);
        counts.push_back(partSize);
        offsets.push_back(offset);
    }

    std::vector<int> part(counts[proc_rank]);
    MPI_Scatterv(vector.data(), counts.data(), offsets.data(), MPI_INT,
                 part.data(), counts[proc_rank], MPI_INT, 0, MPI_COMM_WORLD);

    print_vector(part);

    bool isSorted = true;
    for (int i = 0; i < part.size() - 1; ++i) {
        if (part[i] > part[i + 1]) {
            isSorted = false;
            break;
        }
    }

    bool resSorted;
    MPI_Reduce(&isSorted,&resSorted, 1, MPI_C_BOOL, MPI_LAND, 0, MPI_COMM_WORLD);

    if (proc_rank == 0) {
        if (resSorted) {
            printf("true\n");
        } else {
            printf("false\n");
        }
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

void print_vector(const std::vector<int>& vec) {
    for(int el : vec) {
        std::cout << el << " ";
    }
    std::cout << std::endl;
}