#pragma once
#include <array>
#include <string>
#include <map>
#include <vector>
#include <math.h>

namespace Sudoku {
    enum {
        eSize = 9,
        eBoardSize = eSize * eSize,
        eConstraints = eBoardSize * 4,
        eBufferSize = eConstraints * eSize,
    };
    const int BOX_SIZE = sqrt((unsigned int)eSize);

    typedef unsigned int value;
    typedef std::array<std::array<value, eSize>, eSize> puzzle;
    class DancingLinkTables;
    struct DancingLink;

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

    void calculateConstraintColumns(int columns[4], int row, int col, int num);

    bool isSafe(puzzle grid, int row, int col, int val);
    puzzle fromString(std::string string);

    /**
    * @brief Dancing link which is a quadruply linked list
    * 
    */
    struct DancingLink {
        DancingLink *up;
        DancingLink *down;
        DancingLink *left;
        DancingLink *right;
        DancingLink *colHeader;
        int count; // is the column in columns, otherwise is the position and value in the board

        void cover();
        void uncover();
    };

    class DancingLinkTables {
        private:
        DancingLink root;
        DancingLink colHeaders[eConstraints];
        DancingLink buffer[eBufferSize];
        DancingLink *current[eBoardSize];
        DancingLink *solution[eBoardSize];
        int current_idx;
        int solution_idx;
        int solution_count;
        bool should_randomize;
        bool backTrack();
        void removeGivens();

        public:
        DancingLinkTables(bool randomize);

        void generate(difficulty diff);
        void resetLinks();
        void coverGivens(puzzle grid);

        Sudoku::puzzle createPuzzle();
        Sudoku::puzzle createSolutionPuzzle();
        bool isUnique() const;

        void solve();
    };


}