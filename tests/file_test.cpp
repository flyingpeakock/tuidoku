#include <gtest/gtest.h>
#include <fstream>
#include "file_test.h"

/**
 * @brief sets the function to be tested
 * 
 * @param f the function
 */
void FileTest::setFunc(std::vector<SimpleBoard> (*f)(std::istream&)) {
    func = f;
}

/**
 * @brief run the tests
 * 
 * @param test_table pointer to the start of the test table
 * @param size size of the test table
 */
void FileTest::runTests(file_test_t *test_table, size_t size) {
    for (auto i = 0; i < size; i++) {
        std::ifstream file;
        file.open((test_table + i)->fileName);
        auto puzzle = func(file);
        ASSERT_GT(puzzle.size(), 0);
        EXPECT_EQ(puzzle[0].getPlayGrid(), buildPuzzle((test_table + i)->puzzle));
    }
}

/**
 * @brief function that turns 9x9 c style array into 2d std::array
 * 
 * @param array c style array that should be transformed
 * @return puzzle
 */
Sudoku::puzzle buildPuzzle(int array[9][9]) {
    Sudoku::puzzle ret;
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            ret[i][j] = array[i][j];
        }
    }
    return ret;
}

/**
 * @brief Unit test for getSDKPuzzle which parses sdk files for puzzles
 * 
 */
TEST_F(FileTest, getSDKPuzzle) {
    file_test_t test_table[] = {
        {
            TEST_PUZZLES_ROOT_DIR "sdkpuzzle1.sdk",
            {
                {2, 0, 0, 1, 0, 5, 0, 0, 3},
                {0, 5, 4, 0, 0, 0, 7, 1, 0},
                {0, 1, 0, 2, 0, 3, 0, 8, 0},
                {6, 0, 2, 8, 0, 7, 3, 0, 4},
                {0, 0, 0, 0, 0, 0, 0, 0, 0},
                {1, 0, 5, 3, 0, 9, 8, 0, 6},
                {0, 2, 0, 7, 0, 1, 0, 6, 0},
                {0, 8, 1, 0, 0, 0, 2, 4, 0},
                {7, 0, 0, 4, 0, 2, 0, 0, 1}
            }
        },
    };

    this->setFunc(*file::getSDKPuzzle);
    this->runTests(test_table, sizeof(test_table) / sizeof(test_table[0]));
}


/**
 * @brief Unit test for getSDMpuzzle wich parses SDM files for puzzles
 * 
 */
TEST_F(FileTest, getSDMPuzzle) {
    file_test_t test_table[] = {
        {
            TEST_PUZZLES_ROOT_DIR "sdmpuzzle1.sdm",
            {
                {0, 1, 6, 4, 0, 0, 0, 0, 0},
                {2, 0, 0, 0, 0, 9, 0, 0, 0},
                {4, 0, 0, 0, 0, 0, 0, 6, 2},
                {0, 7, 0, 2, 3, 0, 1, 0, 0},
                {1, 0, 0, 0, 0, 0, 0, 0, 3},
                {0, 0, 3, 0, 8, 7, 0, 4, 0},
                {9, 6, 0, 0, 0, 0, 0, 0, 5},
                {0, 0, 0, 8, 0, 0, 0, 0, 7},
                {0, 0, 0, 0, 0, 6, 8, 2, 0}
            }
        },
    };

    this->setFunc(*file::getSDMPuzzle);
    this->runTests(test_table, sizeof(test_table) / sizeof(test_table[0]));
}

TEST_F(FileTest, getSSPuzzle) {
    file_test_t test_table[] = {
        {
            TEST_PUZZLES_ROOT_DIR "sspuzzle1.ss",
            {
                {1, 0, 0, 0, 0, 0, 7, 0, 0},
                {0, 2, 0, 0, 0, 0, 5, 0, 0},
                {6, 0, 0, 3, 8, 0, 0, 0, 0},
                {0, 7, 8, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 6, 0, 9, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 1, 4, 0},
                {0, 0, 0, 0, 2, 5, 0, 0, 9},
                {0, 0, 3, 0, 0, 0, 0, 6, 0},
                {0, 0, 4, 0, 0, 0, 0, 0, 2}
            }
        },
        {
            TEST_PUZZLES_ROOT_DIR "sspuzzle2.ss",
            {
                {0, 6, 0, 1, 0, 4, 0, 5, 0},
                {0, 0, 8, 3, 0, 5, 6, 0, 0},
                {2, 0, 0, 0, 0, 0, 0, 0, 1},
                {8, 0, 0, 4, 0, 7, 0, 0, 6},
                {0, 0, 6, 0, 0, 0, 3, 0, 0},
                {7, 0, 0, 9, 0, 1, 0, 0, 4},
                {5, 0, 0, 0, 0, 0, 0, 0, 2},
                {0, 0, 7, 2, 0, 6, 9, 0, 0},
                {0, 4, 0, 5, 0, 8, 0, 7, 0}
            }
        },
    };

    this->setFunc(*file::getSSPuzzle);
    this->runTests(test_table, sizeof(test_table) / sizeof(test_table[0]));
}

