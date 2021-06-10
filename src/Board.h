#pragma once
#include <vector>
#include <map>
#include <ostream>
#include "Solver.h"

class Board {
    private:
        struct cell {
            int row:5;
            int col:5;
            bool operator<(const cell &r) const {
                return (row < r.row || (row == r.row && col < r.col));
            }
            bool operator==(const cell &r) const {
                return row == r.row && col == r.col;
            }
        };

        puzzle playGrid;
        puzzle startGrid;
        puzzle solutionGrid;

        std::array<std::array<std::vector<char>, 9>, 9> pencilMarks;
        std::array<std::array<std::map<cell, char>, 9>, 9> pencilHistory;

        std::map<int, int> count;

        void removeMarks(char val, int row, int col);
        void restoreMarks(int row, int col);
        void printBoard(puzzle grid, std::ostream &stream);

        bool playing;
    public:
        Board(puzzle startGrid, puzzle finishGrid);
        void startPlaying();
        void stopPlaying();
        bool isPlaying();
        bool isWon();
        bool isRemaining(int val);

        std::array<std::array<std::vector<char>, 9>, 9> &getPencilMarks();
        puzzle &getPlayGrid();
        puzzle &getStartGrid();
        puzzle &getSolution();

        void insert(char val, int row, int col);
        void pencil(char val, int row, int col);

        void printBoard(std::ostream &stream);
        void printBoard();
        void printSolution(std::ostream &stream);
        void printSolution();
        void printStart(std::ostream &stream);
        void printStart();

        void swapStartGrid();
        void swapStartGrid(puzzle solution);
};