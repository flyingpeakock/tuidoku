#include "Play.h"
#include "Sudoku/Sudoku.h"
#include "Tui/Tui.h"
#include "HumanSolve.h"
#include <stdexcept>
#include <cmath>

Play::Play(std::vector<keymap> keymap, Sudoku::puzzle grid, WINDOW *window) : sudoku(grid) , gridWindow(window){
    selectedNum = -1;
    row_idx = 0;
    col_idx = 0;
    hintCounter = 0;
    state = State::INSERT;
    message = "Insert";

    std::map<std::string, int *> inputs_map = {
        {"up", &up_key},
        {"down", &down_key},
        {"left", &left_key},
        {"right", &right_key},
        {"pencil", &pencil_key},
        {"insert", &insert_key},
        {"erase", &erase_key},
        {"fillPencils", &fillpencil_key},
        {"exit", &exit_key},
        {"hint", &hint_key},
    };

    for (const auto &key : keymap) {
        *(inputs_map[key.input]) = key.value;
    }
}

void Play::play() {
    while (!sudoku.isWon()) {
        printBoard();
        int c = getch();

        if (c == KEY_UP || c == up_key) {
            row_idx--;
        }
        else if (c == KEY_DOWN || c == down_key) {
            row_idx++;
        }
        else if (c == KEY_LEFT || c == left_key) {
            col_idx--;
        }
        else if (c == KEY_RIGHT || c == right_key) {
            col_idx++;
        }
        else if (c == pencil_key) {
            state = State::PENCIL;
            message = "Pencil";
        }
        else if (c == insert_key) {
            state = State::INSERT;
            message = "Insert";
        }
        else if (c == KEY_BACKSPACE || c == ' ' || c == erase_key) {
            sudoku.insert(0, row_idx, col_idx);
        }
        else if (c == fillpencil_key) {
            sudoku.autoPencil();
        }
        else if (c == hint_key) {
            hintCounter++;
            showHint();
        }
        else if (c >= '0' && c <= '9') {
            hintCounter = 0;
            selectedNum = c - '0';
            if (state == State::PENCIL) {
                sudoku.pencil(c - '0', row_idx, col_idx);
            }
            else {
                sudoku.insert(c - '0', row_idx, col_idx);
            }
        }
        else if (c == exit_key) {
            return;
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
    Sudoku::puzzle currentGrid = sudoku.getCurrentGrid();
    Sudoku::puzzle startGrid = sudoku.getStartGrid();

    Tui::printOutline(gridWindow);
    Tui::highlightCell(gridWindow, row_idx, col_idx); // clears everything from last loop
    Tui::printPuzzle(gridWindow, currentGrid, A_NORMAL);
    Tui::printPuzzle(gridWindow, startGrid, A_UNDERLINE); // Underline givens
    Tui::addMessage(gridWindow, message);
    for (auto i = 0; i < Sudoku::SIZE; i++) {
        for (auto j = 0; j < Sudoku::SIZE; j++) {
            auto pencilMarks = sudoku.getPencil(i, j);
            if (currentGrid[i][j] == 0 && pencilMarks != 0) {
                Tui::printPencilMark(gridWindow, i, j, pencilMarks);
            }
            if (startGrid[i][j] != 0) continue;
            if (currentGrid[i][j] != 0 && !Sudoku::isSafe(currentGrid, i, j, currentGrid[i][j])) {
                Tui::highlightNum(gridWindow, i, j, currentGrid[i][j], A_BOLD, COLOR_ERROR_NUM);
            }
            else if (currentGrid[i][j] == 0 && pencilMarks != 0) {
                for (auto num = 0; num < Sudoku::SIZE; num++) {
                    if ((pencilMarks & (1 << num)) != 0) {
                        if (!Sudoku::isSafe(currentGrid, i, j, num + 1)) {
                            Tui::highlightNum(gridWindow, i, j, num + 1, A_NORMAL, COLOR_ERROR_NUM);
                        }
                    }
                }
            }
        }
    }
    if (selectedNum != 0) {
        for (auto i = 0; i < Sudoku::SIZE; i++) {
            for (auto j = 0; j < Sudoku::SIZE; j++) {
                auto pencilMarks = sudoku.getPencil(i, j);
                if (startGrid[i][j] == selectedNum) {
                    Tui::highlightNum(gridWindow, i, j, selectedNum, A_UNDERLINE, COLOR_FILLED_NUM);
                }
                else if (currentGrid[i][j] == selectedNum && Sudoku::isSafe(currentGrid, i, j, currentGrid[i][j])) {
                    Tui::highlightNum(gridWindow, i, j,selectedNum, A_NORMAL, COLOR_FILLED_NUM);
                }
                else if ((pencilMarks & (1 << (selectedNum - 1))) != 0 && Sudoku::isSafe(currentGrid, i, j, selectedNum)) {
                    Tui::highlightNum(gridWindow, i, j, selectedNum, A_DIM, COLOR_HIGHLIGH_NUM);
                }
            }
        }
    }
    wrefresh(gridWindow);
}

void Play::showHint() {
    if (hintCounter == 1) {
        hint = solveHuman(sudoku);
        message = hint.hint1;
    }
    else if (hintCounter == 2) {
        message = hint.hint2;
    }
    else {
        if (hint.moves.size() != 0) {
            for (auto &move : hint.moves) {
                move(&sudoku);
            }
            hintCounter = 0;
        }
    }
}
