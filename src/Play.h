#pragma once
#include <map>
#include "Sudoku/Sudoku.h"
#include "Tui/Tui.h"

typedef std::array<std::array<std::uint16_t, Sudoku::SIZE>, Sudoku::SIZE> PencilMarks;

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

    void printBoard();

    void removeMarks(int val, int row, int col);
    void restoreMarks(int row, int col);

    public:
    Play(Sudoku::puzzle grid, WINDOW *window);
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