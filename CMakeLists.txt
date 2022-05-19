cmake_minimum_required(VERSION 3.22)
project(lab_7)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE RELEASE)


# mpic++ -o main main.cpp
# mpiexec -n 2 ./main

add_executable(task_1_a task_1/main_a.cpp)
add_executable(task_1_b task_1/main_b.cpp)
add_executable(task_2 task_2/main.cpp)