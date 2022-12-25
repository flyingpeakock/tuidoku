#include <gtest/gtest.h>
#include "humanSolve_test.h"

/**
 * @brief Unit test for findNakedSingle
 * 
 */
TEST(HumanSolve_test, findNakedSingle) {
    single_digit_human_solve_t test_table[] = {
        {(1 << 7), {'8', 3, 2, &Board::insert}, 0, true, "200150074001000020407600013040200095070480060010000430706000289004907350003000040"},
        {(1 << 5), {'6', 5, 6, &Board::insert}, 0, true, "412736589000000106568010370000050210100000008087090000030070865800000000000908401"},
    };

    for (auto &test : test_table) {
        Move move = {};
        Board board(test.gridString);
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
    single_digit_human_solve_t test_table[] = {
        {(1 << 0), {'1', 6, 2, &Board::insert}, 0, true, "009032000000700000162000000010020560000900000050000107000000403026009000005870000"},
    };

    for (auto &test : test_table) {
        Move move = {};
        Board board(test.gridString);
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
    single_digit_human_solve_t test_table[] = {
        {(1 << 4), {'5', 3, 4, &Board::pencil}, 1, true, "041729030769003402032640719403900170607004903195370024214567398376090541958431267"},
        {(1 << 0), {'1', 1, 3, &Board::pencil}, 9, true, "980062753065003000327050006790030500050009000832045009673591428249087005518020007"},
    };

    for (auto &test : test_table) {
        std::vector<Move> moves;
        Board board(test.gridString);
        board.autoPencil();

        std::stringstream boardstrstream;
        board.printBoard(boardstrstream);
        EXPECT_EQ(findXwing(board, test.num, moves), test.ret) << "Could not find X-Wing:\n" << boardstrstream.str();
        ASSERT_EQ(moves.size(), test.moves);
        if (test.ret == false) continue;

        EXPECT_EQ(moves[0].val, test.move.val);
        EXPECT_EQ(moves[0].row, test.move.row);
        EXPECT_EQ(moves[0].col, test.move.col);
        EXPECT_EQ(moves[0].move, test.move.move);
    }
}

TEST(HumanSolve_test, findUniqueRectangle) {
    single_digit_human_solve_t test_table[] = {
        {((1 << 7) | (1 << 8)), {'8', 1, 1, &Board::pencil}, 2, true, "502008967100700452067500381213657849654891273700004615821900034306000098005083026"},
    };

    for (auto &test : test_table) {
        std::vector<Move> moves;
        Board board(test.gridString);
        board.autoPencil();

        EXPECT_EQ(findUniqueRectangle(board, test.num, moves), test.ret);
        ASSERT_EQ(moves.size(), test.moves);
        if (test.ret == false) continue;

        EXPECT_EQ(moves[0].val, test.move.val);
        EXPECT_EQ(moves[0].row, test.move.row);
        EXPECT_EQ(moves[0].col, test.move.col);
        EXPECT_EQ(moves[0].move, test.move.move);
    }
}

TEST(HumanSolve_test, findChainOfPairs) {
    single_digit_human_solve_t test_table[] = {
        {((1 << 3) | (1 << 4)), {'5', 5, 6, &Board::pencil}, 1, true, "798452316603781092012030870370265048820143760060897023980014237107028050200070081"},
        {((1 << 3) | (1 << 0)), {0, 0, 0, NULL}, 0, false, "798452316603781092012030870370265048820143760060897023980014237107028050200070081"},
        {((1 << 1) | (1 << 6)), {'7', 4, 8, &Board::pencil}, 1, true, "360859004519472386408613950146738295900541000005926401054387009093164500001295043"},
    };

    for (auto &test : test_table) {
        std::vector<Move> moves;
        Board board(test.gridString);
        board.autoPencil();

        std::stringstream boardstrstr;
        board.printBoard(boardstrstr);
        EXPECT_EQ(findChainOfPairs(board, test.num, moves), test.ret) << "No chain found:\n" << boardstrstr.str();
        ASSERT_EQ(moves.size(), test.moves);
        if (test.ret == false) continue;

        EXPECT_EQ(moves[0].val, test.move.val);
        EXPECT_EQ(moves[0].row, test.move.row);
        EXPECT_EQ(moves[0].col, test.move.col);
        EXPECT_EQ(moves[0].move, test.move.move);
    }
}
