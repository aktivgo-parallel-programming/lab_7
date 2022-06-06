//
// Created by vladislav on 02.06.22.
//
#include <iostream>
#include <mpich/mpi.h>
#include <cstdlib>
#include <vector>
#include <random>

std::vector<int> create_vector(int);
void print_vector(const std::vector<int>&);

int main(int argc, char** argv)
{
    srand(time(nullptr));

    int proc_rank, proc_num, n, m;
    std::vector<int> vec1, vec_recv1;
    std::vector<int> vec2, vec_recv2, vec3, vec_recv3;

    int localMin = 1e8, globalMin = 0;
    int localRes = 0, globalRes;

    int color, key;

    MPI_Init(&argc, &argv);

    MPI_Comm new_comm;

    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

    if (proc_rank % 2 == 0)
    {
        key = proc_rank;
        color = 0;
    }
    else
    {
        key = proc_num - proc_rank;
        color = 1;
    }

    MPI_Comm_split(MPI_COMM_WORLD, color, key, &new_comm);
    MPI_Comm_size(new_comm, &proc_num);

    if (proc_rank == 0)
    {
        std::cout << "Enter n: ";
        std::cin >> n;

        if (n < 1 || n > 1e8) {
            printf("wrong n");
            return -1;
        }

        std::cout << "Enter m: ";
        std::cin >> m;

        if (n < 1 || n > 1e8) {
            printf("wrong m");
            return -1;
        }
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vec1 = create_vector(m);
    vec2 = create_vector(n);
    vec3 = create_vector(n);

    if (proc_rank == 0) {
        printf("vec1:\n");
        print_vector(vec1);

        printf("vec2:\n");
        print_vector(vec2);

        printf("vec3:\n");
        print_vector(vec3);
    }

    if (proc_rank % 2 == 0)
    {
        vec_recv1.resize(m / proc_num);
        MPI_Scatter(vec1.data(), m / proc_num, MPI_INT,
                    vec_recv1.data(), m / proc_num, MPI_INT, 0, new_comm);

        for (int el : vec_recv1)
        {
            if (el < localMin) {
                localMin = el;
            }
        }

        MPI_Reduce(&localMin, &globalMin, 1, MPI_INT, MPI_MIN, 0, new_comm);
        if (proc_rank == 0) {
            printf("min = %d\n", globalMin);
        }
    }
    else
    {
        vec_recv2.resize(n / proc_num);
        MPI_Scatter(vec2.data(), n / proc_num, MPI_INT,
                    vec_recv2.data(), n / proc_num, MPI_INT, 0, new_comm);

        vec_recv3.resize(n / proc_num);
        MPI_Scatter(vec3.data(), n / proc_num, MPI_INT,
                    vec_recv3.data(), n / proc_num, MPI_INT, 0, new_comm);

        for (size_t i = 0; i < vec_recv2.size(); i++)
        {
            localRes += vec_recv2[i] * vec_recv3[i];
        }

        MPI_Reduce(&localRes, &globalRes, 1, MPI_INT, MPI_SUM, 1, new_comm);
        if (proc_rank == 1) {
            printf("res = %d\n", globalRes);
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}

std::vector<int> create_vector(int n) {
    std::vector<int> vector;

    for (int i = 0; i < n; ++i) {
        vector.push_back(rand() % 10);
    }

    return vector;
}

void print_vector(const std::vector<int>& vec) {
    for(int el : vec) {
        std::cout << el << " ";
    }
    std::cout << std::endl;
}