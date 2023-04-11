#include "Sudoku.h"
#include "../Play.h"
//#include "../HumanSolve.h"
#include <algorithm>
#include <chrono>
#include <random>

static Sudoku::puzzle removeGivens(Sudoku::puzzle filled);

Sudoku::puzzle Sudoku::generate() {
    return generate(ANY);
}

Sudoku::puzzle Sudoku::generate(difficulty diff) {
    puzzle grid = {};
    unsigned int difficulty = 0;
    solve(grid, true, difficulty);

    grid = removeGivens(grid);

    return grid;
}

static Sudoku::puzzle removeGivens(Sudoku::puzzle filled) {
    Sudoku::puzzle grid = filled;
    struct Cell
    {
        int row;
        int col;
    };

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::array<Cell, Sudoku::SIZE*Sudoku::SIZE> cells;
    int count = 0;
    for (auto i = 0; i < Sudoku::SIZE; i++) {
        for (auto j = 0; j < Sudoku::SIZE; j++) {
            cells[count].row = i;
            cells[count].col = j;
            count++;
        }
    }

    // Shuffle to randomly remove positions
    shuffle(cells.begin(), cells.end(), std::default_random_engine(seed));

    Sudoku::puzzle copy;
    for (const auto &cell : cells) {
        int removed = grid[cell.row][cell.col];
        grid[cell.row][cell.col] = 0;
        copy = grid;
        unsigned int trash;
        bool isUnique = Sudoku::solve(copy, false, trash);
        if (!isUnique) {
            // Removal made it a bad move, put it back
            grid[cell.row][cell.col] = removed;
        }
    }
    return grid;
}
