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

void SimpleBoard::insert(char val, int row, int col) {

    if (val == ERASE_KEY || START_CHAR - 1 == val) {
        if (playGrid[row][col] != 0) {
            playGrid[row].set(col, 0);
        }
        return;
    }

    if (val > START_CHAR - 1 && val <= START_CHAR + 8) {
        playGrid[row].set(col, val - START_CHAR + 1);
    }
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
        for (auto &vec : array) {
            for (auto i = 0; i < 3; i++) {
                vec.push_back(' ');
            }
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

std::array<std::array<std::vector<char>, 9>, 9> &Board::getPencilMarks() {
    return pencilMarks;
}

puzzle &Board::getStartGrid() {
    return startGrid;
}

puzzle &Board::getSolution() {
    return solutionGrid;
}

void Board::insert(char val, int row, int col) {
    if (startGrid[row][col] != 0) {
        // Trying to change a correct checked square
        return;
    }

    if (val == ERASE_KEY || START_CHAR - 1 == val) {
        if (playGrid[row][col] != 0) {
            count[playGrid[row][col]]--;
            playGrid[row].set(col, 0);
            restoreMarks(row, col);
        }
        return;
    }

    if (val > START_CHAR - 1 && val <= START_CHAR + 8) {
        restoreMarks(row, col);
        if (playGrid[row][col] != 0)
            count[playGrid[row][col]]--;
        count[val - '0']++;
        playGrid[row].set(col, val - START_CHAR + 1);
    }

    removeMarks(val, row, col);
}

void Board::autoPencil() {
    for (auto i = 0; i < playGrid.size(); i++) {
        for (auto j = 0; j < playGrid[i].size(); j++) {
            if (playGrid[i][j] > 0) {
                // Number is filled, don't put pencil marks
                continue;
            }
            auto &marks = pencilMarks[i][j];
            marks.clear();
            for (auto k = 0; k < 3; k++) {
                marks.push_back(' ');
            }
            for (auto k = START_CHAR; k < START_CHAR + 9; k++) {
                if (Solver::isSafe(playGrid, i, j, k - START_CHAR + 1)) {
                    marks.insert(marks.begin(), k);
                }
            }
        }
    }
}

void Board::pencil(char val, int row, int col, bool isBig) {
    if (playGrid[row][col] > 0) {
        // Grid not empty, cant pencil here
        return;
    }
    auto &marks = pencilMarks[row][col];
    if (val == ERASE_KEY || START_CHAR - 1 == val) {
        if (marks[0] != ' ') {
            marks.erase(marks.begin());
        }
        return;
    }

    // Check if mark exists, if visible delete
    // else move to front
    int idx = 0;
    for (auto &m : marks) {
        if (m == val) {
            if (idx > 2 && !isBig) {
                marks.erase(marks.begin() + idx);
                marks.insert(marks.begin(), val);
            }
            else {
                marks.erase(marks.begin() + idx);
            }
            return;
        }
        idx++;
    }

    if (val > START_CHAR - 1 && val <= START_CHAR + 8) {
        marks.insert(marks.begin(), val);
    }
}

void Board::removeMarks(char val, int row, int col) {
    if (!REMOVE_MARKS)
        return;
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < pencilMarks[row][i].size(); j++) {
            auto &mark = pencilMarks[row][i];
            if (val == mark[j] && mark[j] != ' ') {
                mark.erase(mark.begin() + j);
                pencilHistory[row][i][cell{row, col}] = val;
                break;
            }
        }
        for (auto j = 0; j < pencilMarks[i][col].size(); j++) {
            auto &mark = pencilMarks[i][col];
            if (val == mark[j] && mark[j] != ' ') {
                mark.erase(mark.begin() + j);
                pencilHistory[i][col][cell{row, col}] = val;
                break;
            }
        }
    }

    int boxRow = (row / 3) * 3;
    int boxCol = (col / 3) * 3;
    for (auto i = boxRow; i < boxRow + 3; i++) {
        for (auto j = boxCol; j < boxCol + 3; j++) {
            auto &mark = pencilMarks[i][j];
            for (auto k = 0; k < mark.size(); k++) {
                if (val == mark[k] && mark[k] != ' ') {
                    mark.erase(mark.begin() + k);
                    pencilHistory[i][j][cell{row, col}] = val;
                    break;
                }
            }
        }
    }
}

void Board::restoreMarks(int row, int col) {
    for (auto i = 0; i < 9; i++) {
        if (pencilHistory[row][i][cell{row, col}]) {
            pencil(pencilHistory[row][i][cell{row, col}], row, i, true);
            pencilHistory[row][i][cell{row, col}] = 0;
        }
        if (pencilHistory[i][col][cell{row, col}]) {
            pencil(pencilHistory[i][col][cell{row, col}], i, col, true);
            pencilHistory[i][col][cell{row, col}] = 0;
        }
    }
    int boxRow = (row / 3) * 3;
    int boxCol = (col / 3) * 3;
    for (auto i = boxRow; i < boxRow + 3; i++) {
        for (auto j = boxCol; j < boxCol + 3; j++){
            if (pencilHistory[i][j][cell{row, col}]) {
                pencil(pencilHistory[i][j][cell{row, col}], i, j, true);
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