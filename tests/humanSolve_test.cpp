#include <gtest/gtest.h>
#include "humanSolve_test.h"
#include "../src/Play.h"

/**
 * @brief Unit test for findNakedSingle
 * 
 */
TEST(HumanSolve_test, findNakedSingle) {
    single_digit_human_solve_t test_table[] = {
        {(1 << 7), {8, 3, 2, Sudoku::BEGINNER, &Play::insert}, 0, true, "200150074001000020407600013040200095070480060010000430706000289004907350003000040"},
        {(1 << 5), {6, 5, 6, Sudoku::BEGINNER, &Play::insert}, 0, true, "412736589000000106568010370000050210100000008087090000030070865800000000000908401"},
    };

    for (auto &test : test_table) {
        std::vector<Move> move = {};
        Play board({}, Sudoku::fromString(test.gridString), NULL);
        board.autoPencil();

        EXPECT_EQ(findNakedSingles(board, move), test.ret);
        if (test.ret == false) continue;

        EXPECT_EQ(move[0].val, test.move.val);
        EXPECT_EQ(move[0].row, test.move.row);
        EXPECT_EQ(move[0].col, test.move.col);
        EXPECT_EQ(move[0].move, test.move.move);
    }
}

/**
 * @brief Unit test for findHiddenSingle
 * 
 */
TEST(HumanSolve_test, findHiddenSingle) {
    single_digit_human_solve_t test_table[] = {
        {(1 << 0), {1, 6, 2, Sudoku::EASY, &Play::insert}, 0, true, "009032000000700000162000000010020560000900000050000107000000403026009000005870000"},
    };

    for (auto &test : test_table) {
        Move move = {};
        Play board({}, Sudoku::fromString(test.gridString), NULL);
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
        {(1 << 4), {5, 3, 4, Sudoku::HARD, &Play::pencil}, 1, true, "041729030769003402032640719403900170607004903195370024214567398376090541958431267"},
        {(1 << 0), {1, 1, 3, Sudoku::HARD, &Play::pencil}, 9, true, "980062753065003000327050006790030500050009000832045009673591428249087005518020007"},
    };

    for (auto &test : test_table) {
        std::vector<Move> moves;
        Play board({}, Sudoku::fromString(test.gridString), NULL);
        board.autoPencil();

        EXPECT_EQ(findXwing(board, test.num, moves), test.ret) << "Could not find X-Wing:\n";
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
        {((1 << 7) | (1 << 8)), {8, 1, 1, Sudoku::HARD, &Play::pencil}, 2, true, "502008967100700452067500381213657849654891273700004615821900034306000098005083026"},
    };

    for (auto &test : test_table) {
        std::vector<Move> moves;
        Play board({}, Sudoku::fromString(test.gridString), NULL);
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
        {((1 << 3) | (1 << 4)), {5, 5, 6, Sudoku::EXPERT, &Play::pencil}, 1, true, "798452316603781092012030870370265048820143760060897023980014237107028050200070081"},
        {((1 << 3) | (1 << 0)), {0, 0, 0, Sudoku::ANY, NULL}, 0, false, "798452316603781092012030870370265048820143760060897023980014237107028050200070081"},
        {((1 << 1) | (1 << 6)), {7, 4, 8, Sudoku::EXPERT, &Play::pencil}, 1, true, "360859004519472386408613950146738295900541000005926401054387009093164500001295043"},
    };

    for (auto &test : test_table) {
        std::vector<Move> moves;
        Play board({}, Sudoku::fromString(test.gridString), NULL);
        board.autoPencil();

        std::stringstream boardstrstr;
        EXPECT_EQ(findChainOfPairs(board, test.num, moves), test.ret) << "No chain found:\n" << boardstrstr.str();
        ASSERT_EQ(moves.size(), test.moves);
        if (test.ret == false) continue;

        EXPECT_EQ(moves[0].val, test.move.val);
        EXPECT_EQ(moves[0].row, test.move.row);
        EXPECT_EQ(moves[0].col, test.move.col);
        EXPECT_EQ(moves[0].move, test.move.move);
    }
}

TEST(HumanSolve_test, findXYwing) {
    struct findXYwingStruct {
        std::string boardString;
        bool foundMove;
        Move move;
    };

    findXYwingStruct test_table[] = {
        {
            "003060821841052000962810005238000056015638902600020138120080507087290014006000280",
            true,
            {3, 8, 0, Sudoku::EXPERT, &Play::pencil}
        },
        {
            "031064752460075813005013964010459637357628491600137528520791306103586209006342105",
            true,
            {2, 3, 2, Sudoku::EXPERT, &Play::pencil}
        },
    };

    for (auto test : test_table) {
        std::vector<Move> moves;
        Play board({}, Sudoku::fromString(test.boardString), NULL);
        board.autoPencil();

        EXPECT_EQ(findXYwing(board, moves), test.foundMove);

        if (!test.foundMove) continue;

        //EXPECT_EQ(moves[0], test.move);
        EXPECT_EQ(moves[0].val, test.move.val);
        EXPECT_EQ(moves[0].row, test.move.row);
        EXPECT_EQ(moves[0].col, test.move.col);
        EXPECT_EQ(moves[0].move, test.move.move);
    }
}

