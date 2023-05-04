cmake_minimum_required(VERSION 3.20)

# Compiler and standard
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME tuidoku)
set(UNIT_TESTS tuidoku_unit_tests)

project(${PROJECT_NAME} CXX)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

# Fetch GoogleTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.13.0
    OVERRIDE_FIND_PACKAGE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
IF (WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
ENDIF()
FetchContent_MakeAvailable(googletest)

# Fetch FTXUI
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v4.0.0
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(SOURCES
    src/main.cpp
    src/Sudoku/Generate.cpp
    src/Sudoku/Solve.cpp
    src/Sudoku/DancingLink.cpp
    src/Sudoku/Sudoku.cpp
    src/Sudoku/Logic.cpp
    #src/Config.cpp
    src/Tui/Tui.cpp
)

set(TEST_SOURCES
    # Add files to be tested here
    src/Sudoku/Solve.cpp
    src/Sudoku/Generate.cpp
    src/Sudoku/DancingLink.cpp
    src/Sudoku/Sudoku.cpp
    src/Sudoku/Logic.cpp
    #src/Config.cpp

    # Add unit tests here
    tests/dancing_links.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME}
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    config++
)

enable_testing()
add_executable(${UNIT_TESTS} ${TEST_SOURCES})
add_compile_definitions(TEST_PUZZLES_ROOT_DIR=\"${PROJECT_SOURCE_DIR}/tests/puzzles/\")
target_link_libraries(${UNIT_TESTS}
    config++
    GTest::gtest_main
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
