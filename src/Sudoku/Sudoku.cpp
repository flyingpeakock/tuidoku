#include "Sudoku.h"
#include <cmath>
#include <string>
#include <iostream>
#include <random>

/**
 * @brief calculate the constraint columns for a given row, columns and number
 * 
 * @param columns array that gets filled with the correct values
 * @param row 0-(eSize - 1)
 * @param col 0-(eSize - 1)
 * @param num 0-(eSize - 1)
 */
void Sudoku::calculateConstraintColumns(int columns[4], int row, int col, int num) {
    const int box_idx = Sudoku::BOX_SIZE * (row / Sudoku::BOX_SIZE) + (col / Sudoku::BOX_SIZE);
    const int constraintSection = Sudoku::eSize * Sudoku::eSize;
    columns[0] = (row * Sudoku::eSize) + col;
    columns[1] = constraintSection + (row * Sudoku::eSize) + num;
    columns[2] = (constraintSection * 2) + (col * Sudoku::eSize) + num;
    columns[3] = (constraintSection * 3) + (box_idx * Sudoku::eSize) + num;
}

bool Sudoku::isSafe(puzzle grid, int row, int col, int num) {
    // Checking in the box
    const int box_size = BOX_SIZE;
    const int box_row = (row / box_size) * box_size;
    const int box_col = (col / box_size) * box_size;

    for (auto i = box_row; i < box_row + box_size; i++) {
        for (auto j = box_col; j < box_col + box_size; j++) {
            if (i == row && j == col) continue;
            if (grid[i][j] == num) return false;
        }
    }

    // Checking in rows and columns
    for (auto i = 0; i < eSize; i++) {
        if (i != row && grid[i][col] == num) return false;
        if (i != col && grid[row][i] == num) return false;
    }

    return true; // All checks passed
}

Sudoku::puzzle Sudoku::fromString(std::string string) {
    if (string.size() != Sudoku::eSize * Sudoku::eSize)
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
        if (col == eSize) {
            col = 0;
            row++;
        }
    }
    return ret;
}

Sudoku::puzzle Sudoku::DancingLinkTables::createPuzzle() {
    puzzle ret;
    for (auto depth = 0; depth < current_idx; depth++) {
        DancingLink *row = current[depth];
        int i = row->count / eBoardSize;
        int j = (row->count % eBoardSize) / eSize;
        int num = (row->count % eSize) + 1;
        ret[i][j] = num;
    }
    return ret;
}

Sudoku::puzzle Sudoku::DancingLinkTables::createSolutionPuzzle() {
    puzzle ret;
    for (auto depth = 0; depth < solution_idx; depth++) {
        DancingLink *row = solution[depth];
        int i = row->count / eBoardSize;
        int j = (row->count % eBoardSize) / eSize;
        int num = (row->count % eSize) + 1;
        ret[i][j] = num;
    }
    return ret;
}
