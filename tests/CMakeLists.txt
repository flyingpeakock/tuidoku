cmake_minimum_required(VERSION 3.20)

# compiler and standard
set(CMAKE_CXX_STANDARD 20)

set(UNIT_TESTS tuidoku_unit_tests)
set(SUDOKU_LIBRARY sudoku)

set(MAIN_SRC_DIR ../src/)

# Find GoogleTest
Find_Package(GTest REQUIRED)

set(TEST_SOURCES
    # Add unit tests here
    dancing_links.cpp
    logic.cpp
)

add_subdirectory(${MAIN_SRC_DIR}/sudoku main_src/sudoku)

enable_testing()
add_executable(${UNIT_TESTS} ${TEST_SOURCES})
target_include_directories(${UNIT_TESTS} PRIVATE ${MAIN_SRC_DIR})
add_compile_definitions(TEST_PUZZLES_ROOT_DIR=\"${PROJECT_SOURCE_DIR}/tests/puzzles/\")
target_link_libraries(${UNIT_TESTS}
    GTest::gtest_main
    ${SUDOKU_LIBRARY}
)
include(GoogleTest)
gtest_discover_tests(${UNIT_TESTS})

add_custom_target(
    run_gtest ALL
    COMMENT "Running unit tests\n"
    DEPENDS ${UNIT_TESTS}
    COMMAND ${UNIT_TESTS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/
)