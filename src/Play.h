#pragma once
#include <map>
#include "Sudoku/Sudoku.h"
#include "Tui/Tui.h"
#include "Config.h"

typedef std::array<std::array<std::uint16_t, Sudoku::SIZE>, Sudoku::SIZE> PencilMarks;

struct Move {
    Sudoku::value val;
    int row;
    int col;
    Sudoku::difficulty difficulty;
    void (Sudoku::SudokuObj::*move)(Sudoku::value, int, int);
    void operator()(Sudoku::SudokuObj *board) {
        if (move == NULL) return;
        return (board->*move)(val, row, col);
    }
    const bool operator==(const Move &m) const{
        return val == m.val && row == m.row && col == m.col && move == m.move;
    }
};

struct Hint {
    std::string hint1;
    std::string hint2;
    std::vector<Move> moves;
};

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

    int hintCounter;
    Hint hint;
    std::string message;

    int up_key, down_key, left_key, right_key, pencil_key, insert_key, erase_key, fillpencil_key, exit_key, hint_key;

    void printBoard();
    void showHint();

    void removeMarks(int val, int row, int col);
    void restoreMarks(int row, int col);

    public:
    struct keymap {
        std::string input;
        int value;
    };

    Play(std::vector<keymap> keys, Sudoku::puzzle grid, WINDOW *window);
    void play();
};