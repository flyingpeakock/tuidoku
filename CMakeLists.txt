cmake_minimum_required(VERSION 3.20)

# Compiler and standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(PROJECT_NAME tuidoku)
set(UNIT_TESTS tuidoku_unit_tests)

project(${PROJECT_NAME} C CXX)

set(SOURCES
    src/File.cpp
    # src/HumanSolve.cpp
    src/main.cpp
    src/Stopwatch.cpp
    src/Sudoku/Generate.cpp
    src/Sudoku/Solve.cpp
    src/Sudoku/Sudoku.cpp
    src/Tui/Tui.cpp
    src/Play.cpp
    src/Config.cpp
)

set(TEST_SOURCES
    # Add files to be tested here
    src/File.cpp
    # src/HumanSolve.cpp
    src/Sudoku/Solve.cpp
    src/Sudoku/Generate.cpp
    src/Sudoku/Sudoku.cpp
    src/Play.cpp
    src/Tui/Tui.cpp
    src/Config.cpp

    # Add unit tests here
    tests/file_test.cpp
    # tests/humanSolve_test.cpp
    tests/dancing_links.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(GTest)
find_package(libconfig REQUIRED)

set(CURSES_NEED_WIDE true)
find_package( Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Threads::Threads
    ${CURSES_LIBRARIES}
    config++
)

enable_testing()
add_executable(${UNIT_TESTS} ${TEST_SOURCES})
add_compile_definitions(TEST_PUZZLES_ROOT_DIR=\"${PROJECT_SOURCE_DIR}/tests/puzzles/\")
target_link_libraries(${UNIT_TESTS}
    # PRIVATE Threads::Threads
    ${CURSES_LIBRARIES}
    config++
    GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(${UNIT_TESTS})

#add_custom_target(
    #run_gtest ALL
    #COMMENT "Running unit tests\n"
    #DEPENDS ${UNIT_TESTS}
    #COMMAND ${UNIT_TESTS}
    #WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/
#)