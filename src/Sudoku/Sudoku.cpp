#include "Sudoku.h"
#include <cmath>
#include <string>
#include <iostream>

bool Sudoku::isSafe(puzzle grid, int row, int col, int num) {
    // Checking in the box
    const int box_size = sqrt(SIZE);
    const int box_row = (row / box_size) * box_size;
    const int box_col = (col / box_size) * box_size;

    for (auto i = box_row; i < box_row + box_size; i++) {
        for (auto j = box_col; j < box_col + box_size; j++) {
            if (i == row && j == col) continue;
            if (grid[i][j] == num) return false;
        }
    }

    // Checking in rows and columns
    for (auto i = 0; i < SIZE; i++) {
        if (i != row && grid[i][col] == num) return false;
        if (i != col && grid[row][i] == num) return false;
    }

    return true; // All checks passed
}

Sudoku::puzzle Sudoku::fromString(std::string string) {
    if (string.size() != Sudoku::SIZE * Sudoku::SIZE)
        throw std::invalid_argument("Not the correct number of characters to create a sudoku board");

    Sudoku::puzzle ret;
    int row = 0;
    int col = 0;
    for (auto i = 0; i < string.size(); i++) {
        if (string[i] > '9' || string[i] < '0') {
            throw std::invalid_argument("Invalid character in puzzle");
        }
        ret[row][col] = string[i] - '0';
        col++;
        if (col == 9) {
            col = 0;
            row++;
        }
    }
    return ret;
}
