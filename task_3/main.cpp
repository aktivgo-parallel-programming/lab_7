//
// Created by vladislav on 02.06.22.
//
#include <iostream>
#include <mpich/mpi.h>
#include <cstdlib>
#include <vector>
#include <chrono>

std::vector<int> create_matrix(int);
std::vector<int> create_vector(int);
void print_matrix(const std::vector<int>&, int n);
void print_vector(const std::vector<int>&);

int main(int argc, char **argv) {
    srand(time(nullptr));

    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

    MPI_Init(&argc, &argv);

    int proc_num;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    int n = 0;
    if (proc_rank == 0) {
        std::cout << "Enter n: ";
        std::cin >> n;

        if (n < 1 || n > 10000) {
            printf("wrong n");
            return -1;
        }
    }

    start = std::chrono::high_resolution_clock::now();

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<int> matrix;
    std::vector<int> vector(n);

    if (proc_rank == 0) {
        matrix = create_matrix(n);
        //print_matrix(matrix, n);
        //printf("\n");

        vector = create_vector(n);
        //print_vector(vector);
        //printf("\n");
    }

    MPI_Bcast(vector.data(), n, MPI_INT, 0, MPI_COMM_WORLD);

    int countRows = n / proc_num;

    std::vector<int> rows(countRows * n);
    MPI_Scatter(matrix.data(), countRows * n, MPI_INT,
                 rows.data(), countRows * n, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<int> result(countRows);
    for(int i = 0; i < countRows; i++) {
        int sum = 0;
        for (int j = 0; j < n; j++) {
            sum += rows[j + i * n] * vector[j];
        }
        result[i] = sum;
    }

    //print_vector(result);

    std::vector<int> allResult(n);
    MPI_Gather(result.data(), countRows, MPI_INT,
               allResult.data(), countRows, MPI_INT, 0, MPI_COMM_WORLD);

    if (proc_rank == 0) {
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;

        double time = diff.count();
        std::cout << "Time: " << time << "s" << std::endl;

        //printf("res:\n");
        //print_vector(allResult);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}

std::vector<int> create_matrix(int n) {
    std::vector<int> matrix;

    for (int i = 0; i < n; ++i) {
        auto row = create_vector(n);
        for (int j = 0; j < n; ++j) {
            matrix.push_back(row[j]);
        }
    }

    return matrix;
}

std::vector<int> create_vector(int n) {
    std::vector<int> vector;

    for (int i = 0; i < n; ++i) {
        vector.push_back(rand() % 10);
    }

    return vector;
}

void print_matrix(const std::vector<int>& matrix, int n) {
    for (int i = 0; i < matrix.size(); ++i) {
        if (i != 0 && i % n == 0) {
            printf("\n");
        }
        printf("%d ", matrix[i]);
    }

    printf("\n");
}

void print_vector(const std::vector<int>& vec) {
    for(int el : vec) {
        std::cout << el << " ";
    }
    std::cout << std::endl;
}