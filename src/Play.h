#pragma once
#include <map>
#include "Sudoku/Sudoku.h"
#include "Tui/Tui.h"
#include "Config.h"

typedef std::array<std::array<std::uint16_t, Sudoku::SIZE>, Sudoku::SIZE> PencilMarks;

class Play {
    private:
    enum State {
        INSERT,
        PENCIL
    };

    Sudoku::SudokuObj sudoku;
    WINDOW *gridWindow;
    int selectedNum;
    int row_idx;
    int col_idx;
    State state;

    std::string message;

    int up_key, down_key, left_key, right_key, pencil_key, insert_key, erase_key, fillpencil_key, exit_key;

    void printBoard();

    public:
    struct keymap {
        std::string input;
        int value;
    };

    Play(std::vector<keymap> keys, Sudoku::puzzle grid, WINDOW *window);
    void play();
};