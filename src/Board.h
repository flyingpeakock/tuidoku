#pragma once
#include <array>
#include <vector>
#include <map>
#include <iosfwd>
#include <cstdint>

typedef std::array<std::array<int, 9>, 9> puzzle;

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
        void printBoard(puzzle grid, std::ostream &stream) const;

    public:
        SimpleBoard(puzzle startGrid);
        void startPlaying();
        void stopPlaying();
        bool isPlaying() const;
        bool isEmpty(int row, int col) const;

        const puzzle &getPlayGrid() const;

        virtual bool insert(char val, int row, int col);

        void printBoard(std::ostream &stream) const;
        void printBoard() const;
};

class Board : public SimpleBoard {
    private:
        puzzle startGrid;
        puzzle solutionGrid;

        std::array<std::array<std::uint16_t, 9>, 9> pencilMarks;
        std::array<std::array<std::map<cell, char>, 9>, 9> pencilHistory;

        std::map<char, char> count;

        void removeMarks(char val, int row, int col);
        void restoreMarks(int row, int col);

    public:
        Board(puzzle startGrid, puzzle finishGrid);
        bool isWon();
        bool isRemaining(char val) const;

        const std::array<std::array<std::uint16_t, 9>, 9> &getPencilMarks();
        const puzzle &getStartGrid() const;
        const puzzle &getSolution() const;

        bool insert(char val, int row, int col);
        bool pencil(const char val, int row, int col);
        uint16_t getPencil(char row, char col) const;
        void autoPencil();

        void printSolution(std::ostream &stream) const;
        void printSolution() const;
        void printStart(std::ostream &stream) const;
        void printStart() const;

        void swapStartGrid();
        void swapStartGrid(puzzle solution);
};
