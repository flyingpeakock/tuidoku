#pragma once
#include <array>
#include <string>
#include <map>
#include <vector>
#include <math.h>

namespace Sudoku {
    enum {
        SIZE = 9,
        CONSTRAINTS = SIZE * SIZE * 4,
        CHOICES = SIZE * SIZE * SIZE,
    };
    const int BOX_SIZE = sqrt((unsigned int)SIZE);

    typedef unsigned int value;
    typedef std::array<std::array<value, SIZE>, SIZE> puzzle;
    class SudokuObj;

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

        void cover() {
            DancingLink *i;
            DancingLink *j;
            right->left = left;
            left->right = right;

            for (i = down; i != (this); i = i->down) {
                for (j = i->right; j != i; j = j->right) {
                    j->down->up = j->up;
                    j->up->down = j->down;
                    j->colHeader->count--;
                }
            }
        }

        void uncover() {
            DancingLink *i;
            DancingLink *j;
            for (i = up; i != (this); i = i->up) {
                for (j = i->left; j != i; j = j->left) {
                    j->colHeader->count++;
                    j->down->up = j;
                    j->up->down = j;
                }
            }
            right->left = (this);
            left->right = (this);
        }
    };

    struct DancingLinkTables {
        DancingLink root;
        DancingLink colHeaders[CONSTRAINTS];
        DancingLink buffer[CONSTRAINTS * SIZE];
        DancingLink *current[SIZE * SIZE];
        DancingLink *solution[SIZE * SIZE];
        int current_idx;
        int solution_idx;
        int solution_count;

        Sudoku::puzzle createPuzzle();
        Sudoku::puzzle createSolutionPuzzle();
    };


    void linkPuzzle(bool randomize, DancingLink buffer[CONSTRAINTS * SIZE], DancingLink *root, DancingLink columns[CONSTRAINTS]);
    void calculateConstraintColumns(int columns[4], int row, int col, int num);
    void coverGivens(puzzle &grid, DancingLink *columns);

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