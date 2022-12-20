#pragma once

#include "../src/File.h"
#include "../src/Board.h"

/**
 * @brief struct that holds data for creating puzzles
 * 
 */
struct file_test_t {
    std::string fileName;
    int puzzle[9][9];
};

class FileTest : public ::testing::Test {
    protected:
        std::vector<SimpleBoard> (*func)(std::istream&);
    public:
        void setFunc(std::vector<SimpleBoard> (*f)(std::istream&));
        void runTests(file_test_t *test_table, size_t size);
};

puzzle buildPuzzle(int array[9][9]);
