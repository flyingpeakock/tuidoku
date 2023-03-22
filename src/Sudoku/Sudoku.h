#pragma once
#include <array>
#include <string>

namespace Sudoku {
    const int SIZE = 9;

    enum difficulty {
        BEGINNER = 0,
        EASY,
        MEDIUM,
        HARD,
        EXPERT,
        ANY,
        HIGHEST = EXPERT,
    };

    typedef std::array<std::array<int, SIZE>, SIZE> puzzle;

    bool solve(puzzle &grid, bool randomize);
    bool solve(puzzle &grid);
    puzzle generate(difficulty diff);
    puzzle generate();
    bool isSafe(puzzle grid, int row, int col, int val);
    puzzle fromString(std::string string);
}