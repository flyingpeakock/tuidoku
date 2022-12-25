#pragma once
#include <array>

namespace Sudoku {
    const int SIZE = 9;
    typedef std::array<std::array<int, SIZE>, SIZE> puzzle;
    bool solve(puzzle &grid, bool randomize);
    bool solve(puzzle &grid);
    puzzle generate(int unknowns);
    puzzle generate();
    bool isSafe(puzzle grid, int row, int col, int val);
}