#include "Sudoku.h"
#include <algorithm>
#include <chrono>
#include <random>

Sudoku::puzzle Sudoku::generate() {
    return generate(0);
}
Sudoku::puzzle Sudoku::generate(int unknown) {
    if (unknown > 64) {
        unknown = 64;
    }
    if (unknown < 0) {
        unknown = 0;
    }
    puzzle grid = {};

    struct Cell
    {
        int row;
        int col;
    };

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    /*
    std::array<int, 9> firstRow;
    for (auto i = 1; i <= 9; i++) {
        firstRow[i - 1] = i;
    }
    shuffle(firstRow.begin(), firstRow.end(), std::default_random_engine(seed));
    grid[0] = firstRow;
    */
    solve(grid, true);
    std::array<Cell, SIZE*SIZE> cells;
    int count = 0;
    for (auto i = 0; i < SIZE; i++) {
        for (auto j = 0; j < SIZE; j++) {
            cells[count].row = i;
            cells[count].col = j;
            count++;
        }
    }

    // Shuffle to randomly remove positions
    shuffle(cells.begin(), cells.end(), std::default_random_engine(seed));

    int i = 0;
    puzzle copy;
    for (const auto &cell : cells) {
        int removed = grid[cell.row][cell.col];
        grid[cell.row][cell.col] = 0;
        copy = grid;
        bool isUnique = solve(copy, false);
        if (!isUnique) {
            // Removal made it a bad move, put it back
            grid[cell.row][cell.col] = removed;
        }
        else {
            i++;
        }
        if (unknown != 0 && i >= unknown) {
            break;
        }
    }
    if (unknown != 0 && i < unknown) { // Could't find a puzzle with the given unknowns
        return generate(unknown); // trying again
    }
    return grid;
}