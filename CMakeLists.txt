cmake_minimum_required(VERSION 3.22)
project(HashTable C)
set(CMAKE_CXX_COMPILER cc)
set(CMAKE_C_STANDARD 99)

add_library(HashTable library.c)

add_executable(Test library.c library.h)
