#pragma once
#include <array>
#include <string>

namespace Sudoku {
    const int SIZE = 9;

    enum difficulty {
        ANY,
        BEGINNER, // 30 - 40 unknowns
        EASY,     // 40 - 45
        MEDIUM,   // 45 - 50
        HARD,     // 50- 56
        EXPERT    // 57 - 60
    };

    typedef std::array<std::array<int, SIZE>, SIZE> puzzle;

    bool solve(puzzle &grid, bool randomize);
    bool solve(puzzle &grid);
    puzzle generate(int unknowns);
    puzzle generate();
    bool isSafe(puzzle grid, int row, int col, int val);
    puzzle fromString(std::string string);
}