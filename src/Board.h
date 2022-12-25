#pragma once
#include <array>
#include <vector>
#include <map>
#include <iosfwd>
#include <cstdint>
#include "Sudoku/Sudoku.h"

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
        Sudoku::puzzle playGrid;
        bool playing;
        void printBoard(Sudoku::puzzle grid, std::ostream &stream) const;

    public:
        SimpleBoard(Sudoku::puzzle startGrid);
        SimpleBoard(std::string gridString);
        void startPlaying();
        void stopPlaying();
        bool isPlaying() const;
        bool isEmpty(int row, int col) const;

        const Sudoku::puzzle &getPlayGrid() const;

        virtual bool insert(char val, int row, int col);

        void printBoard(std::ostream &stream) const;
        void printBoard() const;
};

class Board : public SimpleBoard {
    private:
        Sudoku::puzzle startGrid;
        Sudoku::puzzle solutionGrid;

        std::array<std::array<std::uint16_t, 9>, 9> pencilMarks;
        std::array<std::array<std::map<cell, char>, 9>, 9> pencilHistory;

        std::map<char, char> count;

        void removeMarks(char val, int row, int col);
        void restoreMarks(int row, int col);

    public:
        Board(Sudoku::puzzle startGrid);
        Board(Sudoku::puzzle startGrid, Sudoku::puzzle finishGrid);
        Board(std::string gridString);
        bool isWon();
        bool isRemaining(char val) const;

        const std::array<std::array<std::uint16_t, 9>, 9> &getPencilMarks();
        const Sudoku::puzzle &getStartGrid() const;
        const Sudoku::puzzle &getSolution() const;

        bool insert(char val, int row, int col);
        bool pencil(const char val, int row, int col);
        uint16_t getPencil(char row, char col) const;
        void autoPencil();

        void printSolution(std::ostream &stream) const;
        void printSolution() const;
        void printStart(std::ostream &stream) const;
        void printStart() const;

        void swapStartGrid();
        void swapStartGrid(Sudoku::puzzle solution);
};
