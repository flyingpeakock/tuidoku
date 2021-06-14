#pragma once
#include "Row.h"

class Solver {
    private:
        puzzle grid;
        puzzle solution;
        bool backtrack(int row, int col);
        int solutions;
        static const int SIZE = 9;
    public:
        Solver();
        Solver(puzzle board);
        Solver(int board[9][9]);
        Solver(int **board);
        Solver(std::array<std::array<int, 9>, 9> board);
        Solver(const char *board);
        bool isSafe(int row, int col, int num);
        puzzle getGrid();
        void solve();
        bool isUnique();
        void changeGrid(puzzle g);
};