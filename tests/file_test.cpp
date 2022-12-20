#include <gtest/gtest.h>
#include "file_test.h"
#include <fstream>

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
 * @brief Unit test for getSDKPuzzle which reads a puzzle of type sdk
 * 
 */
TEST(file_read_test, getSDKPuzzle) {

    std::string fileName1 = TEST_PUZZLES_ROOT_DIR "sdkpuzzle1.sdk";
    std::ifstream file;
    file.open(fileName1);

    int sdk_test_ret_1[9][9] = {
        {2, 0, 0, 1, 0, 5, 0, 0, 3},
        {0, 5, 4, 0, 0, 0, 7, 1, 0},
        {0, 1, 0, 2, 0, 3, 0, 8, 0},
        {6, 0, 2, 8, 0, 7, 3, 0, 4},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 5, 3, 0, 9, 8, 0, 6},
        {0, 2, 0, 7, 0, 1, 0, 6, 0},
        {0, 8, 1, 0, 0, 0, 2, 4, 0},
        {7, 0, 0, 4, 0, 2, 0, 0, 1}
    };

    auto sdk_test_puzzle_1 = file::getSDKPuzzle(file);
    ASSERT_EQ(sdk_test_puzzle_1.size(), 1);
    EXPECT_EQ(sdk_test_puzzle_1[0].getPlayGrid(), buildPuzzle(sdk_test_ret_1));
}
