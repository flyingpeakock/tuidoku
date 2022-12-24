#include "dancing_links.h"
#include "../src/config.h"
#include <gtest/gtest.h>
#include <sstream>

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
    puzzle grid = {};
    bool res = Sudoku::solve(grid);
    EXPECT_EQ(res, false);
    EXPECT_NE(grid, puzzle());
    printBoard(grid, std::cout);
}

TEST(dancingLinks, can_solve) {
    Board board = Generator().createBoard();
    puzzle grid = board.getPlayGrid();
    bool res = Sudoku::solve(grid);
    EXPECT_EQ(res, true);
    EXPECT_EQ(board.getSolution(), grid);
}

TEST(dancingLinks, can_generate) {
    puzzle grid = Sudoku::generate();
    printBoard(grid, std::cout);
    Solver solver(grid);
    solver.solve();
    EXPECT_TRUE(solver.isUnique());
}

TEST(dancingLinks, can_generate_given_unknowns) {
    for (auto i = 1; i < 45; i++) {
        puzzle grid = Sudoku::generate(i);
        printBoard(grid, std::cout);

        int unknowns = 0;
        for (auto &r : grid) {
            for (auto &c : r) {
                if (c == 0) unknowns++;
            }
        }
        EXPECT_EQ(unknowns, i);
    }
}
