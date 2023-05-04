cmake_minimum_required(VERSION 3.16)

# Compiler and standard
set(CMAKE_CXX_STANDARD 20)

set(LIBRARY_NAME Sudoku)

set(LIBRARY_SOURCES
    Generate.cpp
    Solve.cpp
    DancingLink.cpp
    Sudoku.cpp
    Logic.cpp
)

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES})