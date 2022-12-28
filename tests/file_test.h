#pragma once

#include "../src/File.h"
#include "../src/Sudoku/Sudoku.h"

/**
 * @brief struct that holds data for creating puzzles
 * 
 */
struct file_test_t {
    std::string fileName;
    //int puzzle[9][9];
    Sudoku::puzzle puzzle;
};

class FileTest : public ::testing::Test {
    protected:
        std::vector<Sudoku::puzzle> (*func)(std::istream&);
    public:
        void setFunc(std::vector<Sudoku::puzzle> (*f)(std::istream&));
        void runTests(file_test_t *test_table, size_t size);
};

Sudoku::puzzle buildPuzzle(int array[9][9]);
