#pragma once
#include "Generator.h"
#include <vector>
#include <map>
#include <ostream>

class Board {
    private:
        struct cell {
            int row;
            int col;
            bool operator<(const cell &r) const {
                return (row < r.row || (row == r.row && col < r.col));
            }
            bool operator==(const cell &r) const {
                return row == r.row && col == r.col;
            }
        };

        std::array<std::array<int, 9>, 9> playGrid;
        std::array<std::array<int, 9>, 9> startGrid;
        std::array<std::array<int, 9>, 9> solutionGrid;

        std::array<std::array<std::vector<char>, 9>, 9> pencilMarks;
        std::array<std::array<std::map<cell, char>, 9>, 9> pencilHistory;

        std::map<int, int> count;

        void removeMarks(char val, int row, int col);
        void restoreMarks(int row, int col);

        bool playing;
    public:
        Board(Generator gen);
        void startPlaying();
        void stopPlaying();
        bool isPlaying();
        bool isWon();
        bool isRemaining(int val);

        std::array<std::array<std::vector<char>, 9>, 9> &getPencilMarks();
        std::array<std::array<int, 9>, 9> &getPlayGrid();
        std::array<std::array<int, 9>, 9> &getStartGrid();
        std::array<std::array<int, 9>, 9> &getSolution();

        void insert(char val, int row, int col);
        void pencil(char val, int row, int col);

        void printBoard(std::ostream &stream);
        void printBoard();
};