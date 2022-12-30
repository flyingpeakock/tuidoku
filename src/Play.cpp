#include "Play.h"
#include "Sudoku/Sudoku.h"
#include "Tui/Tui.h"
#include <stdexcept>
#include <cmath>

Play::Play(Sudoku::puzzle grid, WINDOW *window) : startGrid(grid), currentGrid(grid), solutionGrid(grid), gridWindow(window) {
    startGrid = currentGrid = solutionGrid = grid;
    if (!Sudoku::solve(solutionGrid)) {
        throw std::invalid_argument("Puzzle does not have a unique solution");
    }
    pencilMarks = {};
    pencilHistory = {};
    count = {};
    selectedNum = -1;
    row_idx = 0;
    col_idx = 0;
    state = INSERT;
    for (auto &i : currentGrid) {
        for (auto &j: i) {
            if (j == 0) continue;
            count[j - 1]++;
        }
    }
}

void Play::play() {
    while (currentGrid != solutionGrid) {
        printBoard();
        int c = getch();
        
        switch (c) {
            case KEY_UP:
            case 'k':
                row_idx--;
                break;
            case KEY_DOWN:
            case 'j':
                row_idx++;
                break;
            case KEY_LEFT:
            case 'h':
                col_idx--;
                break;
            case KEY_RIGHT:
            case 'l':
                col_idx++;
                break;
            case 'p':
                state = PENCIL;
                break;
            case 'i':
                state = INSERT;
                break;
            case 'q':
                return;
            case KEY_BACKSPACE:
            case 127:
                insert(0, row_idx, col_idx);
                break;
            case 'a':
                autoPencil();
                break;
            default:
                if (c >= '0' && c <= '9') {
                    selectedNum = c - '0';
                    if (state == PENCIL) {
                        pencil(c - '0', row_idx, col_idx);
                    }
                    else {
                        insert(c - '0', row_idx, col_idx);
                    }
                }
                break;
        }

        if (col_idx < 0) {
            col_idx = Sudoku::SIZE - 1;
        }
        else if (col_idx >= Sudoku::SIZE) {
            col_idx = 0;
        }
        if (row_idx < 0) {
            row_idx = Sudoku::SIZE - 1;
        }
        else if (row_idx >= Sudoku::SIZE) {
            row_idx = 0;
        }
    }
    printBoard();
    getch();
}

void Play::printBoard() {
    Tui::printOutline(gridWindow);
    Tui::highlightCell(gridWindow, row_idx, col_idx); // clears everything from last loop
    Tui::printPuzzle(gridWindow, currentGrid, A_NORMAL);
    Tui::printPuzzle(gridWindow, startGrid, A_UNDERLINE); // Underline givens
    if (state == PENCIL) {
        Tui::addMessage(gridWindow, "Pencil");
    }
    else {
        Tui::addMessage(gridWindow, "Insert");
    }
    for (auto i = 0; i < Sudoku::SIZE; i++) {
        for (auto j = 0; j < Sudoku::SIZE; j++) {
            if (currentGrid[i][j] == 0 && pencilMarks[i][j] != 0) {
                Tui::printPencilMark(gridWindow, i, j, pencilMarks[i][j]);
            }
        }
    }
    if (selectedNum != 0) {
        for (auto i = 0; i < Sudoku::SIZE; i++) {
            for (auto j = 0; j < Sudoku::SIZE; j++) {
                if (startGrid[i][j] == selectedNum) {
                    Tui::highlightNum(gridWindow, i, j, selectedNum, A_UNDERLINE, COLOR_FILLED_NUM);
                }
                else if (currentGrid[i][j] == selectedNum) {
                    Tui::highlightNum(gridWindow, i, j,selectedNum, A_NORMAL, COLOR_FILLED_NUM);
                }
                else if ((pencilMarks[i][j] & (1 << (selectedNum - 1))) != 0) {
                    Tui::highlightNum(gridWindow, i, j, selectedNum, A_DIM, COLOR_HIGHLIGH_NUM);
                }
            }
        }
    }
    wrefresh(gridWindow);
}

void Play::insert(int val, int row, int col) {
    if (startGrid[row][col] != 0) {
        return; // Cannot change a given grid
    }

    restoreMarks(row, col);
    if (currentGrid[row][col] != 0 && val != currentGrid[row][col]) {
        count[currentGrid[row][col] - 1]--; // removing an occurence of this value
    }
    count[val - 1]++;
    currentGrid[row][col] = val;
    removeMarks(val, row, col);
}

void Play::removeMarks(int val, int row, int col) {
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

void Play::restoreMarks(int row, int col) {
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

void Play::pencil(int val, int row, int col) {
    if (currentGrid[row][col] != 0) 
        return;     // Cannot pencil on a filled box
    
    if (val == 0)
        return;     // Cannot pencil a zero

    // Since val 1-9 we need it 0-8
    val--;

    pencilMarks[row][col] ^= (1 << val);
}

void Play::autoPencil() {
    for (auto i = 0; i < Sudoku::SIZE; i++) {
        for (auto j = 0; j < Sudoku::SIZE; j++) {
            if (currentGrid[i][j] > 0) {
                continue;
            }

            auto &marks = pencilMarks[i][j];
            marks = 0;
            for (int num = 1; num <= Sudoku::SIZE; num++) {
                if (Sudoku::isSafe(currentGrid, i, j, num)) {
                    marks |= (1 << (num - 1));
                }
            }
        }
    }
}

bool Play::isEmpty(int row, int col) {
    return currentGrid[row][col] == 0;
}

std::uint16_t Play::getPencil(int row, int col) {
    return pencilMarks[row][col];
}

bool Play::isWon() {
    return currentGrid == solutionGrid;
}
