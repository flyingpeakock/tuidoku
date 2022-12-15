#include "Solver.h"
#include "Board.h"
#include "config.h"
#include <iostream>
#include <sstream>

SimpleBoard::SimpleBoard(puzzle startGrid) : playGrid(startGrid) {
}

void SimpleBoard::startPlaying() {
    playing = true;
}

void SimpleBoard::stopPlaying() {
    playing = false;
}

bool SimpleBoard::isPlaying() {
    return playing;
}

puzzle &SimpleBoard::getPlayGrid() {
    return playGrid;
}

bool SimpleBoard::insert(char val, int row, int col) {

    if (val == ERASE_KEY || START_CHAR - 1 == val) {
        if (playGrid[row][col] != 0) {
            playGrid[row][col] = 0;
        }
        return true;
    }

    if (val > START_CHAR - 1 && val <= START_CHAR + 8) {
        playGrid[row][col] = val - START_CHAR + 1;
        return true;
    }
    return false;
}

void SimpleBoard::printBoard(puzzle grid, std::ostream &stream) {
    std::stringstream boardStream;
    boardStream << TOPROW << '\n';
    for (auto i = 0; i < 3; i++) {
        boardStream << ROW1 << '\n';
        boardStream << ROW2 << '\n';
        boardStream << ROW1 << '\n';
        boardStream << ROW2 << '\n';
        boardStream << ROW1 << '\n';
        if (i != 2)
            boardStream << ROW3 << '\n';
    }
    boardStream << BOTROW << '\n';
    std::string boardString = boardStream.str();
    int idx = 0;
    for (auto i = 0; i < 9; i++) {
        idx += std::string{TOPROW}.size() + 5;
        for (auto j = 0; j < 9; j++) {
            if (grid[i][j] != 0)
                boardString[idx] = grid[i][j] + '0';
            idx += 6;
        }
    }
    stream << boardString;
}

void SimpleBoard::printBoard() {
    printBoard(playGrid, std::cout);
}

void SimpleBoard::printBoard(std::ostream &stream) {
    printBoard(playGrid, stream);
}

Board::Board(puzzle startGrid,
             puzzle finishGrid
            ) : SimpleBoard(startGrid),
                startGrid(startGrid), 
                solutionGrid(finishGrid) {
    for (auto &array : pencilMarks) {
        for (auto &marks : array) {
            marks = 0;
        }
    }

    for (auto i = 1; i <= 9; i++) {
        count.insert({i, 0});
    }

    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            int val = startGrid[i][j];
            if (val != 0) {
                count[val]++;
            }
        }
    }
}


bool Board::isWon() {
    if (playGrid == solutionGrid) {
        playing = false;
        return true;
    }
    return false;
}

std::array<std::array<std::uint16_t, 9>, 9> &Board::getPencilMarks() {
    return pencilMarks;
}

puzzle &Board::getStartGrid() {
    return startGrid;
}

puzzle &Board::getSolution() {
    return solutionGrid;
}

bool Board::insert(char val, int row, int col) {
    if (startGrid[row][col] != 0) {
        // Trying to change a correct checked square
        return false;
    }

    if (val == ERASE_KEY || START_CHAR - 1 == val) {
        if (playGrid[row][col] != 0) {
            count[playGrid[row][col]]--;
            playGrid[row][col] = 0;
            restoreMarks(row, col);
            return true;
        }
        return false;
    }

    if (val > START_CHAR - 1 && val <= START_CHAR + 8) {
        restoreMarks(row, col);
        if (playGrid[row][col] != 0)
            count[playGrid[row][col]]--;
        count[val - '0']++;
        playGrid[row][col] = val - START_CHAR + 1;
    }

    removeMarks(val, row, col);
    return playGrid[row][col] == solutionGrid[row][col];
}

void Board::autoPencil() {
    for (auto i = 0; i < playGrid.size(); i++) {
        for (auto j = 0; j < playGrid[i].size(); j++) {
            if (playGrid[i][j] > 0) {
                // Number is filled, don't put pencil marks
                continue;
            }
            auto &marks = pencilMarks[i][j];
            marks = 0;
            for (unsigned char k = START_CHAR; k < START_CHAR + 9; k++) {
                if (Solver::isSafe(playGrid, i, j, k - START_CHAR + 1)) {
                    marks |= (1 << (k - START_CHAR));
                }
            }
        }
    }
}

bool Board::pencil(char val, int row, int col) {
    if (playGrid[row][col] > 0) {
        // Grid not empty, cant pencil here
        return false;
    }
    auto &marks = pencilMarks[row][col];
    if (val == ERASE_KEY || START_CHAR - 1 == val) {
        marks = 0;
        return true;
    }

    // removing a correct pencil
    bool ret = !(((marks & (1 << (val - START_CHAR))) != 0) && (playGrid[row][col] == val));
    // toggle the bit
    marks ^= (1u << (val - START_CHAR));
    return ret;
}

void Board::removeMarks(char val, int row, int col) {
    if (!REMOVE_MARKS)
        return;
    for (auto i = 0; i < 9; i++) {
       pencilMarks[row][i] &= ~(1u << (val - START_CHAR));
       pencilMarks[i][col] &= ~(1u << (val - START_CHAR));
    }

    int boxRow = (row / 3) * 3;
    int boxCol = (col / 3) * 3;
    for (auto i = boxRow; i < boxRow + 3; i++) {
        for (auto j = boxCol; j < boxCol + 3; j++) {
            auto &mark = pencilMarks[i][j];
            mark &= ~(1u << (val - START_CHAR));
        }
    }
}

void Board::restoreMarks(int row, int col) {
    for (auto i = 0; i < 9; i++) {
        if (pencilHistory[row][i][cell{row, col}]) {
            pencil(pencilHistory[row][i][cell{row, col}], row, i);
            pencilHistory[row][i][cell{row, col}] = 0;
        }
        if (pencilHistory[i][col][cell{row, col}]) {
            pencil(pencilHistory[i][col][cell{row, col}], i, col);
            pencilHistory[i][col][cell{row, col}] = 0;
        }
    }
    int boxRow = (row / 3) * 3;
    int boxCol = (col / 3) * 3;
    for (auto i = boxRow; i < boxRow + 3; i++) {
        for (auto j = boxCol; j < boxCol + 3; j++){
            if (pencilHistory[i][j][cell{row, col}]) {
                pencil(pencilHistory[i][j][cell{row, col}], i, j);
                pencilHistory[i][j][cell{row, col}] = 0;
            }
        }
    }

}

bool Board::isRemaining(int val) {
    if (val == 0)
        return true;
    return count[val] < 9;
}

void Board::printSolution() {
    printBoard(solutionGrid, std::cout);
}

void Board::printSolution(std::ostream &stream) {
    printBoard(solutionGrid, stream);
}

void Board::printStart() {
    printBoard(startGrid, std::cout);
}

void Board::printStart(std::ostream &stream) {
    printBoard(startGrid, stream);
}

void Board::swapStartGrid() {
    startGrid = playGrid;
}

void Board::swapStartGrid(puzzle solution) {
    startGrid = playGrid;
    solutionGrid = solution;
}