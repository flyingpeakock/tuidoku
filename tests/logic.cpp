#include "logic.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

struct Cell {
    int row;
    int col;
    int num;
};

struct Xlogic_helper_t {
    std::string puzzle;
    std::vector<Cell> truths;
    std::vector<Cell> falses;
    sudoku::logic::moveType result;
    sudoku::constraintColTypes type;
};

void run_xlogic_test(const Xlogic_helper_t &test) {
    auto table = sudoku::generate(test.puzzle);

    sudoku::logic::LogicalMove move;

    int constraints[sudoku::eConstraintTypes];
    sudoku::calculateConstraintColumns(constraints, test.truths[0].row, test.truths[0].col, test.truths[0].num);
    auto constraint = &table.colHeaders->at(constraints[test.type]);

    ASSERT_TRUE(sudoku::isUncovered(constraint));

    auto result = sudoku::logic::xlogic(constraint, move);

    EXPECT_TRUE(result);
    EXPECT_EQ(move.type, test.result);
    EXPECT_EQ(move.truths.size(), test.truths.size());
    EXPECT_EQ(move.truths.size(), test.truths.size());

    for (auto &truth : test.truths) {
        auto found = sudoku::containsLinkEqual(truth.row, truth.col, truth.num, move.truths.begin(), move.truths.end());
        EXPECT_NE(found, move.truths.end());
    }

    for (auto &fal : test.falses) {
        auto found = sudoku::containsLinkEqual(fal.row, fal.col, fal.num, move.falses.begin(), move.falses.end());
        EXPECT_NE(found, move.falses.end());
    }

}

TEST(logic, locked_candidates) {
    Xlogic_helper_t test_table[] = {
        {
            "400270600798156234020840007237468951849531726561792843082015479070024300004087002", // Locked candidates 
            {{6, 0, 2}, {8, 0, 2}},
            {{2, 0, 2}},
            sudoku::logic::eLogicPencil,
            sudoku::eConstraintBox
        },
    };

    for (auto &test : test_table) {
        run_xlogic_test(test);
    }
}

TEST(logic, single) {
    Xlogic_helper_t test_table[] = {
        {
            "009032000000700000162000000010020560000900000050000107000000403026009000005870000", // Hidden single
            {{6, 2, 0}},
            {std::vector<Cell>()},
            sudoku::logic::eLogicInsert,
            sudoku::eConstraintCol
        },
    };
    
    for (auto &test : test_table) {
        run_xlogic_test(test);
    }
}

TEST(logic, doubles) {
    Xlogic_helper_t test_table[] = {
        {
            "801006094300009080970080500547062030632000050198375246083620915065198000219500008", // Hidden double
            {{4, 4, 0}, {4, 5, 0}, {4, 4, 3}, {4, 5, 3}},
            {{4, 3, 3}, {4, 6, 0}, {4, 8, 0}},
            sudoku::logic::eLogicPencil,
            sudoku::eConstraintCell
        },
        {
            "240030001590010320000020004352146897400389512189572643020093100600051009900060030", // Naked double
            {{0, 6, 6}, {0, 6, 8}, {2, 6, 6}, {2, 6, 8}},
            {{0, 7, 6}, {2, 7, 6}, {7, 6, 6}, {7, 6, 6}},
            sudoku::logic::eLogicPencil,
            sudoku::eConstraintCell
        },
    };

    for (auto &test : test_table) {
        run_xlogic_test(test);
    }
}

TEST(logic, triples) {
    Xlogic_helper_t test_table[] = {
        {
            "240030001590010320000020004352146897400389512189572643020093100600051009900060030", // Naked triple
            {{0, 2, 5}, {0, 2, 6}, {0, 2, 7}, {1, 2, 5}, {1, 2, 6}, {1, 2, 7}, {2, 0, 6}, {2, 0, 7}},
            {{2, 1, 5}, {2, 1, 6}, {2, 2, 5}, {2, 2, 6}, {2, 2, 7}},
            sudoku::logic::eLogicPencil,
            sudoku::eConstraintCell
        },
    };

    for (auto &test : test_table) {
        run_xlogic_test(test);
    }
}

TEST(logic, fish) {
    Xlogic_helper_t test_table[] = {
        {
            "608090107079300026000067000000603070706000200080700060805030742047008610102070908", // X-wing
            {{1, 4, 7}, {1, 6, 7}, {3, 4, 7}, {3, 6, 7}},
            {{4, 4, 7}, {2, 6, 7}},
            sudoku::logic::eLogicPencil,
            sudoku::eConstraintRow
        },
    };

    for (auto &test : test_table) {
        run_xlogic_test(test);
    }
}

TEST(logic, conjugates) {
    Xlogic_helper_t test_table[] = {
        {
            "390001700005400001000027000907205160150070009246819537000160000400703610000000073", // Naked Conjugate
            {{0, 3, 5}, {0, 3, 4}, {0, 4, 4}, {0, 4, 7}, {1, 5, 5}, {1, 5, 7}},
            {{2, 3, 4}, {2, 3, 5}, {1, 4, 7}, {0, 7, 4}, {0, 8, 4}},
            sudoku::logic::eLogicPencil,
            sudoku::eConstraintCell
        },
    };

    for (auto &test : test_table) {
        run_xlogic_test(test);
    }
}
