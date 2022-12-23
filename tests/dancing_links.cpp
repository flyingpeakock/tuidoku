#include "dancing_links.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(dancingLinks, solve_empty) {
    puzzle grid = {};
    Sudoku::solve(grid);
    grid = Sudoku::getSolution();
    EXPECT_NE(grid, puzzle());
}

TEST(dancingLinks, can_solve) {
    Board board = Generator().createBoard();
    Sudoku::solve(board.getPlayGrid());
    EXPECT_EQ(board.getSolution(), Sudoku::getSolution());
}