cmake_minimum_required(VERSION 3.5)
project(OpenMP)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

SET(CMAKE_C_COMPILER /usr/local/bin/mpicc)
SET(CMAKE_CXX_COMPILER /usr/local/bin/mpicxx)

set(SOURCE_FILES main.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /Users/brianzhao/ClionProjects/OpenMP/Binaries)
add_executable(OpenMP ${SOURCE_FILES})
