cmake_minimum_required(VERSION 3.20)

# Compiler and standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME tuidoku)
set(UNIT_TESTS tuidoku_unit_tests)

project(${PROJECT_NAME} C CXX)

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

set(TEST_SOURCES
    #Add files to be tested here
    src/File.cpp
    src/Generator.cpp
    src/Solver.cpp
    src/Board.cpp

    #Add unit tests here
    tests/file_test.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(GTest REQUIRED)

find_package( Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Threads::Threads
    ${CURSES_LIBRARIES}
)

enable_testing()
add_executable(${UNIT_TESTS} ${TEST_SOURCES})
add_compile_definitions(TEST_PUZZLES_ROOT_DIR=\"${PROJECT_SOURCE_DIR}/tests/puzzles/\")
target_link_libraries(${UNIT_TESTS}
    # PRIVATE Threads::Threads
    # ${CURSES_LIBRARIES}
    GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(${UNIT_TESTS})