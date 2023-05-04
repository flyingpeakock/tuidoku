cmake_minimum_required(VERSION 3.16)

# Compiler and standard
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME tuidoku)
set(SUDOKU_LIBRARY Sudoku)

project(${PROJECT_NAME} CXX)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

# Fetch FTXUI
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v4.0.0
)

FetchContent_GetProperties(ftxui)
IF(NOT ftxui_POPULATED)
    FetchContent_Populate(ftxui)
    add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
ENDIF()

set(SOURCES
    src/main.cpp
    src/Tui/Tui.cpp
)

add_subdirectory(src/Sudoku)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME}
    PRIVATE ${SUDOKU_LIBRARY}
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)
