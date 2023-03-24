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

Sudoku::SudokuObj::SudokuObj(puzzle grid) : startGrid(grid), currentGrid(grid), solutionGrid(grid) {
    if (!solve(solutionGrid)) {
        throw std::invalid_argument("Puzzle not does not have a valid solution");
    }
    pencilMarks = {};
    pencilHistory = {};
}

void Sudoku::SudokuObj::insert(Sudoku::value val, int row, int col) {
    if (startGrid[row][col] != 0) {
        throw std::invalid_argument("Puzzle contains a clue at this location");
    }

    if ((val < 0) || (val > SIZE)) {
        throw std::invalid_argument("Value is out of the range for this puzzle");
    }

    restoreMarks(row, col);
    currentGrid[row][col] = val;
    removeMarks(val, row, col);
}

void Sudoku::SudokuObj::pencil(Sudoku::value val, int row, int col) {
    if (startGrid[row][col] != 0) {
        throw std::invalid_argument("Puzzle contains a clue at this location");
    }
    if (currentGrid[row][col] != 0) {
        throw std::invalid_argument("Can only place pencil marks in empty cells");
    }

    if ((val < 1) || (val > SIZE)) {
        throw std::invalid_argument("Value is out of range for this puzzle");
    }

    pencilMarks[row][col] ^= (1 << (val - 1));
}

void Sudoku::SudokuObj::autoPencil() {
    for (auto i = 0; i < SIZE; i++) {
        for (auto j = 0; j < Sudoku::SIZE; j++) {
            if (currentGrid[i][j] != 0) {
                continue;
            }
            auto &marks = pencilMarks[i][j];
            marks = 0;
            for (int num = 1; num <= SIZE; num++) {
                if (Sudoku::isSafe(currentGrid, i, j, num)) {
                    marks |= (1 << (num - 1));
                }
            }
        }
    }
}

void Sudoku::SudokuObj::restoreMarks(int row, int col) {
    int idx = (row * Sudoku::SIZE) + col;
    for (auto i = 0; i < 9; i++) {
        if (pencilHistory[row][i][idx] != 0) {
            pencil(pencilHistory[row][i][idx], row, i);
            pencilHistory[row][i][idx] = 0;
        }
        if (pencilHistory[i][col][idx] != 0) {
            pencil(pencilHistory[i][col][idx], i, col);
            pencilHistory[i][col][idx] = 0;
        }
    }

    int boxSize = sqrt(Sudoku::SIZE);
    int boxRow = (row / boxSize) * boxSize;
    int boxCol = (col / boxSize) * boxSize;
    for (auto i = boxRow; i < boxRow + boxSize; i++) {
        for (auto j = boxCol; j < boxCol + boxSize; j++) {
            if (pencilHistory[i][j][idx] != 0) {
                pencil(pencilHistory[i][j][idx], i, j);
                pencilHistory[i][j][idx] = 0;
            }
        }
    }
}

void Sudoku::SudokuObj::removeMarks(Sudoku::value val, int row, int col) {
    if (val == 0)
        return;
    // Since val is between 1-9 we need it 0-8
    val--;
    // Removing this mark from rows and cols
    for (auto i = 0; i < 9; i++) {
        if ((pencilMarks[row][i] & (1 << val)) != 0) {
            pencilHistory[row][i][(row * Sudoku::SIZE) + col] = val + 1;
            pencilMarks[row][i] &= ~(1 << val);
        }
        if ((pencilMarks[i][col] & (1 << val)) != 0) {
            pencilHistory[i][col][(row * Sudoku::SIZE) + col] = val + 1;
            pencilMarks[i][col] &= ~(1 << val);
        }
    }

    int boxSize = sqrt(Sudoku::SIZE);
    int boxRow = (row / boxSize) * boxSize;
    int boxCol = (col / boxSize) * boxSize;
    for (auto i = boxRow; i < boxRow + boxSize; i++) {
        for (auto j = boxCol; j < boxCol + boxSize; j++) {
            if ((pencilMarks[i][j] & (1 << val)) != 0) {
                pencilHistory[i][j][(row * Sudoku::SIZE) + col] = val + 1;
                pencilMarks[i][j] &= ~(1 << val);
            }
        }
    }
}

bool Sudoku::SudokuObj::isEmpty(int row, int col) const {
    return (currentGrid[row][col] == 0);
}

bool Sudoku::SudokuObj::isWon() const {
    return (currentGrid == solutionGrid);
}

Sudoku::value Sudoku::SudokuObj::getPencil(int row, int col) const {
    return pencilMarks[row][col];
}

Sudoku::value Sudoku::SudokuObj::getValue(int row, int col) const {
    return currentGrid[row][col];
}

Sudoku::value Sudoku::SudokuObj::getAnswer(int row, int col) const {
    return solutionGrid[row][col];
}

Sudoku::value Sudoku::SudokuObj::getStartValue(int row, int col) const {
    return startGrid[row][col];
}

Sudoku::puzzle Sudoku::SudokuObj::getStartGrid() const {
    return startGrid;
}

Sudoku::puzzle Sudoku::SudokuObj::getCurrentGrid() const {
    return currentGrid;
}

Sudoku::puzzle Sudoku::SudokuObj::getSolutionGrid() const {
    return solutionGrid;
}
