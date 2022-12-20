#include <gtest/gtest.h>
#include "humanSolve_test.h"

/**
 * @brief Unit test for findNakedSingle
 * 
 */
TEST(HumanSolve_test, findNakedSingle) {
    struct findNakedSingle_t {
        std::uint16_t num;
        Move move;
        bool ret;
        const char *gridString;
    };

    findNakedSingle_t test_table[] = {
        {(1 << 7), {'8', 3, 2, &Board::insert}, true, "200150074001000020407600013040200095070480060010000430706000289004907350003000040"},
        {(1 << 5), {'6', 5, 6, &Board::insert}, true, "412736589000000106568010370000050210100000008087090000030070865800000000000908401"},
    };

    for (auto &test : test_table) {
        Move move = {};
        Generator gen(test.gridString);
        Board board = gen.createBoard();
        board.autoPencil();

        EXPECT_EQ(findNakedSingles(board, test.num, &move), test.ret);
        if (test.ret == false) continue;

        EXPECT_EQ(move.val, test.move.val);
        EXPECT_EQ(move.row, test.move.row);
        EXPECT_EQ(move.col, test.move.col);
        EXPECT_EQ(move.move, test.move.move);
    }
}