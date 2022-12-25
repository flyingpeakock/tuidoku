#include "dancing_links.h"
#include "../src/config.h"
#include <gtest/gtest.h>
#include <sstream>

void GenerateBoard(std::string puzzleString, std::string solutionString, puzzle &grid, puzzle &solution) {
    for (auto i = 0; i < Sudoku::SIZE; i++) {
        for (auto j = 0; j < Sudoku::SIZE; j++) {
            auto str_index = (i * Sudoku::SIZE) + j;
            grid[i][j] = puzzleString[str_index] - '0';
            solution[i][j] = solutionString[str_index] - '0';
        }
    }
}

void printBoard(puzzle grid, std::ostream &stream) {
    std::stringstream boardStream;
    boardStream << TOPROW << '\n';
    for (auto i = 0; i < 3; i++) {
        boardStream << ROW1 << '\n';
        boardStream << ROW2 << '\n';
        boardStream << ROW1 << '\n';
        boardStream << ROW2 << '\n';
        boardStream << ROW1 << '\n';
        if (i != 2)
            boardStream << ROW3 << '\n';
    }
    boardStream << BOTROW << '\n';
    std::string boardString = boardStream.str();
    int idx = 0;
    for (auto i = 0; i < 9; i++) {
        idx += std::string{TOPROW}.size() + 5;
        for (auto j = 0; j < 9; j++) {
            if (grid[i][j] != 0)
                boardString[idx] = grid[i][j] + '0';
            idx += 6;
        }
    }
    stream << boardString;
}

TEST(dancingLinks, solve_empty) {
    for (auto i = 0; i < 1000; i++) {
        puzzle grid = {};
        bool res = Sudoku::solve(grid, false);
        EXPECT_EQ(res, false);
        EXPECT_NE(grid, puzzle());
        printBoard(grid, std::cout);
    }
}

TEST(dancingLinks, can_solve) {
    struct puzzle_solution_str {
        std::string puzzle;
        std::string solution;
    };

    puzzle_solution_str test_table[] = {
        {
            "004300209005009001070060043006002087190007400050083000600000105003508690042910300",
            "864371259325849761971265843436192587198657432257483916689734125713528694542916378"
        },
        {
            "040100050107003960520008000000000017000906800803050620090060543600080700250097100",
            "346179258187523964529648371965832417472916835813754629798261543631485792254397186"
        },
        {
            "600120384008459072000006005000264030070080006940003000310000050089700000502000190",
            "695127384138459672724836915851264739273981546946573821317692458489715263562348197"
        },
        {
            "497200000100400005000016098620300040300900000001072600002005870000600004530097061",
            "497258316186439725253716498629381547375964182841572639962145873718623954534897261"
        },
        {
            "005910308009403060027500100030000201000820007006007004000080000640150700890000420",
            "465912378189473562327568149738645291954821637216397854573284916642159783891736425"
        },
        {
            "100005007380900000600000480820001075040760020069002001005039004000020100000046352",
            "194685237382974516657213489823491675541768923769352841215839764436527198978146352"
        },
    };

    for (auto &test : test_table) {
        puzzle grid;
        puzzle solution;
        GenerateBoard(test.puzzle, test.solution, grid, solution);
        bool ret = Sudoku::solve(grid, false);
        EXPECT_TRUE(ret);
        EXPECT_EQ(grid, solution);
    }
}

TEST(dancingLinks, generate_then_solve) {
    puzzle grid = Sudoku::generate(0);
    puzzle solved = grid;
    Sudoku::solve(solved, false);
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 0; j++) {
            if (grid[i][j] == 0) continue;
            EXPECT_EQ(grid[i][j], solved[i][j]);
        }
    }
}

TEST(dancingLinks, generate_numb_of_unknowns) {
    for (auto unknowns = 1; unknowns < 60; unknowns++) {
        puzzle grid = Sudoku::generate(unknowns);
        auto un = 0;
        for (auto &i : grid) {
            for (auto &j : i) {
                if (j == 0) {
                    un++;
                }
            }
        }
        EXPECT_EQ(un, unknowns);
        printBoard(grid, std::cout);
    }
}

TEST(dancingLinks, generate_not_too_slow) {
    for (auto i = 0; i < 0xFF; i++) {
        puzzle grid = Sudoku::generate();
        printBoard(grid, std::cout);
    }
}