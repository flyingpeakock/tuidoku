#include "Solver.h"
#include <algorithm>
#include <random>
#include <chrono>
#include <string.h>
#include <stdexcept>

#define SIZE 9

int row::operator[](int i) {
    switch(i) {
        case 0:
            return col1;
        case 1:
            return col2;
        case 2:
            return col3;
        case 3:
            return col4;
        case 4:
            return col5;
        case 5:
            return col6;
        case 6:
            return col7;
        case 7:
            return col8;
        case 8:
            return col9;
    }
    return -1;
}

void row::set(int col, int val) {
    switch(col) {
        case 0:
            col1 = val;
            return;
        case 1:
            col2 = val;
            return;
        case 2:
            col3 = val;
            return;
        case 3:
            col4 = val;
            return;
        case 4:
            col5 = val;
            return;
        case 5:
            col6 = val;
            return;
        case 6:
            col7 = val;
            return;
        case 7:
            col8 = val;
            return;
        case 8:
            col9 = val;
            return;
    }
}

int row::size() {
    return 9;
}

/*const bool row::operator==(const row &r) {
    return (col1 == r.col1 && col2 == r.col2 && col3 == r.col3 &&
            col4 == r.col4 && col5 == r.col5 && col6 == r.col6 &&
            col7 == r.col7 && col8 == r.col8 && col9 == r.col9);
}*/

Solver::Solver() {
    solutions = 0;
    for (auto i = 0; i < SIZE; i++) {
        for (auto j = 0; j < SIZE; j++) {
            grid[i].set(j, 0);
        }
    }
}

Solver::Solver(puzzle board) {
    solutions = 0;
    grid = board;
}

Solver::Solver(int board[9][9]) {
    solutions = 0;
    for (auto i = 0; i < SIZE; i++) {
        for (auto j = 0; j < SIZE; j++) {
            grid[i].set(j, board[i][j]);
        }
    }
}

Solver::Solver(int **board) {
    solutions = 0;
    for (auto i = 0; i < SIZE; i++) {
        for (auto j = 0; j < SIZE; j++) {
            grid[i].set(j, board[i][j]);
        }
    }
}

Solver::Solver(std::array<std::array<int, 9>, 9> board) {
    solutions = 0;
    for (auto i = 0; i < board.size(); i++) {
        for (auto j = 0; j < board[i].size(); j++) {
            grid[i].set(j, board[i][j]);
        }
    }
}

Solver::Solver(const char *board) {
    solutions = 0;
    auto idx = 0;
    if (strlen(board) != 81)
        throw std::invalid_argument("Wrong amount of digits to make a 9x9 sudoku");
    for (auto i = 0; i < SIZE; i++) {
        for (auto j = 0; j < SIZE; j++) {
            grid[i].set(j, board[idx++] - '0');
        }
    }
}

puzzle Solver::getGrid() {
    return solution;
}

bool Solver::isSafe(int row, int col, int num) {

    // Check for same numb in same row
    for (auto i = 0; i < SIZE; i++) {
        if (grid[row][i] == num) return false;
    }

    // Check for same numb in same col
    for (auto i = 0; i < SIZE; i++) {
        if (grid[i][col] == num) return false;
    }

    // Check for same numb in same box
    int boxRow = (row / 3) * 3;
    int boxCol = (col / 3) * 3;
    for (auto i = boxRow; i < boxRow + 3; i++) {
        for (auto j = boxCol; j < boxCol + 3; j++) {
            if (grid[i][j] == num) return false;
        }
    }
    
    return true; // All checks passed
}


bool Solver::backtrack(int row, int col) {
    // Has reached the end of the board, return true
    if (row == SIZE - 1 && col == SIZE) {
        solutions++;
        solution = grid;
        return true;
    }

    // Recahed the end of row, go to next
    if (col == SIZE) {
        col = 0;
        row++;
    }

    // Position already has value, go to next
    if (grid[row][col] > 0) {
        return backtrack(row, col + 1);
    }

    // Here comes the fun part
    // Create array with randomness instead of normal loop
    // used to generate unique puzzles
    std::array<int, 9> nums {1,2,3,4,5,6,7,8,9};
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (nums.begin(), nums.end(), std::default_random_engine(seed));

    //for (auto num = 1; num <= SIZE; num++) {
    for (auto num : nums) {
        // Check if safe to place num
        if (isSafe(row, col, num)) {
            grid[row].set(col, num);
            if (backtrack(row, col + 1) && solutions > 1)
            //if (backtrack(row, col +1))
                return true;
        }
    }
    // Number was wrong, reset
    grid[row].set(col, 0);
    return false;
}

void Solver::solve() {
    backtrack(0, 0);
}

bool Solver::isUnique() {
    return solutions == 1;
}

void Solver::changeGrid(puzzle g) {
    solutions = 0;
    grid = g;
}