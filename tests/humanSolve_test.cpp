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

/**
 * @brief Unit test for findHiddenSingle
 * 
 */
TEST(HumanSolve_test, findHiddenSingle) {
    struct findHiddenSingle_t {
        std::uint16_t num;
        Move move;
        bool ret;
        const char *gridString;
    };

    findHiddenSingle_t test_table[] = {
        {(1 << 0), {'1', 6, 2, &Board::insert}, true, "009032000000700000162000000010020560000900000050000107000000403026009000005870000"},
    };

    for (auto &test : test_table) {
        Move move = {};
        Generator gen(test.gridString);
        Board board = gen.createBoard();
        board.autoPencil();

        EXPECT_EQ(findHiddenSingles(board, test.num, &move), test.ret);
        if (test.ret == false) continue;

        EXPECT_EQ(move.val, test.move.val);
        EXPECT_EQ(move.row, test.move.row);
        EXPECT_EQ(move.col, test.move.col);
        EXPECT_EQ(move.move, test.move.move);
    }
}

TEST(HumanSolve_test, findXwing) {
    struct findXwing_t {
        std::uint16_t num;
        Move move;
        int moves;
        bool ret;
        const char *gridString;
    };

    findXwing_t test_table[] = {
        {(1 << 4), {'5', 3, 4, &Board::pencil}, 1, true, "041729030769003402032640719403900170607004903195370024214567398376090541958431267"},
        {(1 << 0), {'1', 1, 3, &Board::pencil}, 9, true, "980062753065003000327050006790030500050009000832045009673591428249087005518020007"},
    };

    for (auto &test : test_table) {
        std::vector<Move> moves;
        Generator gen(test.gridString);
        Board board = gen.createBoard();
        board.autoPencil();

        std::stringstream boardstrstream;
        board.printBoard(boardstrstream);
        EXPECT_EQ(findXwing(board, test.num, moves), test.ret) << "Could not find X-Wing:\n" << boardstrstream.str();
        if (test.ret == false) continue;

        ASSERT_EQ(moves.size(), test.moves);
        EXPECT_EQ(moves[0].val, test.move.val);
        EXPECT_EQ(moves[0].row, test.move.row);
        EXPECT_EQ(moves[0].col, test.move.col);
        EXPECT_EQ(moves[0].move, test.move.move);
    }
}
