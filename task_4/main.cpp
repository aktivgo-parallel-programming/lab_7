//
// Created by vladislav on 02.06.22.
//
#include <iostream>
#include <mpich/mpi.h>
#include <cstdlib>
#include <vector>

std::vector<int> create_matrix(int);
std::vector<int> create_vector(int);
void print_matrix(const std::vector<int>&, int n);
void print_vector(const std::vector<int>&);

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int proc_num;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

    int proc_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    int n = 0;
    int m = 0;
    std::vector<int> vector1;
    std::vector<int> vector2;
    std::vector<int> vector3;

    if (proc_rank == 0) {
        std::cout << "Enter n: ";
        std::cin >> n;

        if (n < 1 || n > 1e8) {
            printf("wrong n");
            return -1;
        }

        std::cout << "Enter m: ";
        std::cin >> m;

        if (n < 1 || n > 1e8) {
            printf("wrong n");
            return -1;
        }

        vector1 = create_vector(m);
        print_vector(vector1);
        printf("\n");

        vector2 = create_vector(n);
        print_vector(vector2);
        printf("\n");

        vector3 = create_vector(n);
        print_vector(vector3);
        printf("\n");
    }

    int colour;
    int key;
    if(proc_rank % 2 == 0)
    {
        colour = 0;
        key = proc_rank;
    }
    else
    {
        colour = 1;
        key = proc_num - proc_rank;
    }

    // Split de global communicator
    MPI_Comm new_comm;
    MPI_Comm_split(MPI_COMM_WORLD, colour, key, &new_comm);

    // Get my rank in the new communicator
    int my_new_comm_rank;
    MPI_Comm_rank(new_comm, &my_new_comm_rank);

    int my_new_comm_num;
    MPI_Comm_size(new_comm, &my_new_comm_num);

    if (proc_rank % 2 == 0) {
        std::vector<int> rows(m / my_new_comm_num);
        MPI_Scatter(vector1.data(), m / my_new_comm_num, MPI_INT,
                    rows.data(), m / my_new_comm_num, MPI_INT, 0, new_comm);

        int min = 1e8;
        for (int row : rows) {
            if (row < min) {
                min = row;
            }
        }

        int globMin;
        MPI_Reduce(&min,&globMin, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

        if (my_new_comm_rank == 0) {
            printf("min = %d\n", globMin);
        }
    } else {

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
        vector.push_back(i);
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