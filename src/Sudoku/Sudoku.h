#pragma once
#include <array>
#include <string>
#include <map>
#include <vector>

namespace Sudoku {
    const int SIZE = 9;

    /**
     * @brief Difficulty can be of a move or of an entire puzzle
     *        A puzzle is the same difficulty as it's most difficult move
     * 
     */
    enum difficulty {
        MISTAKE = 0,
        BEGINNER = 1,
        EASY = 2,
        MEDIUM = 3,
        HARD = 4,
        EXPERT = 5,
        ANY = 6,
        LOWEST = BEGINNER,
        HIGHEST = EXPERT,
    };

    typedef unsigned int value;
    typedef std::array<std::array<value, SIZE>, SIZE> puzzle;
    class SudokuObj;

    bool solve(puzzle &grid, bool randomize, unsigned int &difficulty);
    bool solve(puzzle &grid);
    puzzle generate(difficulty diff);
    puzzle generate();
    bool isSafe(puzzle grid, int row, int col, int val);
    puzzle fromString(std::string string);

    class SudokuObj {
        private:
        puzzle startGrid;
        puzzle currentGrid;
        puzzle solutionGrid;
        puzzle pencilMarks;
        std::array<std::array<std::map<value, value>, SIZE>, SIZE> pencilHistory;

        void restoreMarks(int row, int col);
        void removeMarks(value val, int row, int col);

        public:
        SudokuObj(puzzle grid);

        void insert(value val, int row, int col);
        void pencil(value val, int row, int col);
        void autoPencil();

        bool isEmpty(int row, int col) const;
        bool isWon() const;

        value getPencil(int row, int col) const;
        value getValue(int row, int col) const;
        value getAnswer(int row, int col) const;
        value getStartValue(int row, int col) const;

        puzzle getStartGrid() const;
        puzzle getCurrentGrid() const;
        puzzle getSolutionGrid() const;
    };
}