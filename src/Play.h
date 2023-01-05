#pragma once
#include <map>
#include "Sudoku/Sudoku.h"
#include "Tui/Tui.h"
#include "Config.h"

typedef std::array<std::array<std::uint16_t, Sudoku::SIZE>, Sudoku::SIZE> PencilMarks;

class Play;

struct Move {
    int val;
    int row;
    int col;
    void (Play::*move)(int, int, int);
    void operator()(Play *board) {
        if (move == NULL) return;
        return (board->*move)(val, row, col);
    }
};

struct Hint {
    std::string hint1;
    std::string hint2;
    int difficulty;
    std::vector<Move> moves;
};

class Play {
    private:
    enum State {
        INSERT,
        PENCIL
    };

    Sudoku::puzzle startGrid;
    Sudoku::puzzle currentGrid;
    Sudoku::puzzle solutionGrid;
    PencilMarks pencilMarks;
    std::array<std::array<std::map<int, int>, Sudoku::SIZE>, Sudoku::SIZE> pencilHistory;
    std::array<int, Sudoku::SIZE> count;
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
        keys input;
        int value;
    };

    Play(std::vector<keymap> keys, Sudoku::puzzle grid, WINDOW *window);
    void play();

    
    void insert(int val, int row, int col);
    void pencil(int val, int row, int col);
    void autoPencil();

    bool isEmpty(int row, int col);
    bool isWon();

    std::uint16_t getPencil(int row, int col);

    /*
    void autoPencil();
    */
};