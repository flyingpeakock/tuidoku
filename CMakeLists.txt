cmake_minimum_required(VERSION 3.16)

# Compiler and standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME tuidoku)

project(${PROJECT_NAME} C CXX)

enable_testing()

set(SOURCES
    src/Arguments.cpp
    src/Board.cpp
    src/File.cpp
    src/Game.cpp
    src/Generator.cpp
    src/HumanSolve.cpp
    src/main.cpp
    src/Solver.cpp
    src/Stopwatch.cpp
    src/Window.cpp
    )

add_executable(${PROJECT_NAME} ${SOURCES})

find_package( Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads ${CURSES_LIBRARIES})