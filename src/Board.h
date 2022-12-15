#pragma once
#include <vector>
#include <map>
#include <iosfwd>
#include "Row.h"
#include <cstdint>

class SimpleBoard {
    protected:
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
        bool playing;
        void printBoard(puzzle grid, std::ostream &stream);

    public:
        SimpleBoard(puzzle startGrid);
        void startPlaying();
        void stopPlaying();
        bool isPlaying();

        puzzle &getPlayGrid();

        virtual bool insert(char val, int row, int col);

        void printBoard(std::ostream &stream);
        void printBoard();
};

class Board : public SimpleBoard {
    private:
        puzzle startGrid;
        puzzle solutionGrid;

        std::array<std::array<std::uint16_t, 9>, 9> pencilMarks;
        std::array<std::array<std::map<cell, char>, 9>, 9> pencilHistory;

        std::map<int, int> count;

        void removeMarks(char val, int row, int col);
        void restoreMarks(int row, int col);

    public:
        Board(puzzle startGrid, puzzle finishGrid);
        bool isWon();
        bool isRemaining(int val);

        std::array<std::array<std::uint16_t, 9>, 9> &getPencilMarks();
        puzzle &getStartGrid();
        puzzle &getSolution();

        bool insert(char val, int row, int col);
        bool pencil(char val, int row, int col);
        void autoPencil();

        void printSolution(std::ostream &stream);
        void printSolution();
        void printStart(std::ostream &stream);
        void printStart();

        void swapStartGrid();
        void swapStartGrid(puzzle solution);
};