#include <gtest/gtest.h>
#include "file_test.h"
#include <fstream>

/**
 * @brief struct that holds data for creating puzzles
 * 
 */
struct file_test_t {
    std::string fileName;
    int puzzle[9][9];
};

/**
 * @brief function that turns 9x9 c style array into 2d std::array
 * 
 * @param array c style array that should be transformed
 * @return puzzle
 */
puzzle buildPuzzle(int array[9][9]) {
    puzzle ret;
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
TEST(file_test, getSDKPuzzle) {
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

    for (auto & test : test_table) {
        std::ifstream file;
        file.open(test.fileName);
        auto puzzle = file::getSDKPuzzle(file);
        ASSERT_GT(puzzle.size(), 0);
        EXPECT_EQ(puzzle[0].getPlayGrid(), buildPuzzle(test.puzzle));
    }
}

/**
 * @brief Unit test for getSDMpuzzle wich parses SDM files for puzzles
 * 
 */
TEST(file_test, getSDMPuzzle) {
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

    for (auto &test : test_table) {
        std::ifstream file;
        file.open(test.fileName);
        auto puzzle = file::getSDMPuzzle(file);
        ASSERT_GT(puzzle.size(), 0);
        EXPECT_EQ(puzzle[0].getPlayGrid(), buildPuzzle(test.puzzle));
    }
}

TEST(file_test, getSSPuzzle) {
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

    for (auto &test : test_table) {
        std::ifstream file;
        file.open(test.fileName);
        auto puzzle = file::getSSPuzzle(file);
        ASSERT_GT(puzzle.size(), 0);
        EXPECT_EQ(puzzle[0].getPlayGrid(), buildPuzzle(test.puzzle));
    }
}
