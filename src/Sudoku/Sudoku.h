#pragma once
#include <array>
#include <string>
#include <map>
#include <vector>

namespace Sudoku {
    enum {
        eSize = 9,
        eBoxSize = 3,
        eBoardSize = eSize * eSize,
        eConstraints = eBoardSize * 4,
        eBufferSize = eConstraints * eSize,
    };

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
        bool isUnique() const;

        void solve();
    };


}