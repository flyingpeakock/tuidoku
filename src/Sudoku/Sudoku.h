#pragma once
#include "Constants.h"
#include "DancingLink.h"
#include <random>

namespace Sudoku {

    /**
     * @brief enums of board difficulties
     * 
     */
    enum difficulty {
        eBeginner,
        eEasy,
        eMedium,
        eHard,
        eExpert,
        ePro,
        eDifficulties,
        eHighestDifficulty = ePro,
        eAny
    };

    /**
     * @brief solves a sudoku puzzle
     * 
     * @param table .. puzzle represented as a constraint table
     *                 the table should be fully created
     * @param randomize .. randomize for creating puzzles 
     * @return true .. if a unique solution
     * @return false .. if none or multiple solutions
     */
    bool solve(DancingLinkTable &table, bool randomize);

    /**
     * @brief Generate a sudoku puzzle with a unique solution
     * 
     * @param diff difficulty of board to generate
     * @return DancingLinkTable 
     */
    DancingLinkTable generate(difficulty diff);

    DancingLinkTable generate(std::string string);

    DancingLinkTable generate();

    enum moveType {
        eCoverFull,
        eCoverRow
    };

    struct Move {
        moveType type;
        DancingLink *link;
    };

    struct SudokuPuzzle {
        SudokuPuzzle(Sudoku::DancingLinkTable &table);
        Sudoku::DancingLinkTable constraintTable;
        int current_start_index;
        Sudoku::DancingLinkContainer wrong_inputs;
        Sudoku::DancingLinkContainer pencilMarks;   // contains visible marks
        Sudoku::DancingLinkContainer wrong_marks;   // contains visible marks that are wrong
        Sudoku::DancingLinkContainer removed_marks; // contains marks that have been visible but no longer are
        std::vector<Move> moves;

        void insert(int row, int col, char num);
        void pencil(int row, int col, char num);
        void autoPencil();

        void recheckMistakes(Sudoku::DancingLink *link);
    };

    /**
    * @brief Removed a link from the SudokuPuzzle type
    *
    * @param puzzle puzzle to remove from
    * @param row 0 - 8
    * @param col 0 - 8
    */
    void removeFromPuzzle(SudokuPuzzle *puzzle, int row, int col);

    bool canSee(DancingLink *link_l, DancingLink *link_r);


    /**
    * @brief find the column with the fewest amount of rows
    * 
    * @return ColHeader 
    */
    static DancingLinkColumn *smallestColumn(Sudoku::DancingLink *root, bool randomize) {
        Sudoku::DancingLinkColumn *current;
        Sudoku::DancingLinkColumn *ret;
        int min = 0xFFFF;
        if (!randomize) {
            for (current = (Sudoku::DancingLinkColumn *)root->right; (Sudoku::DancingLink *)current != root; current = (Sudoku::DancingLinkColumn*)current->right) {
                if (current->count < min) {
                    min = current->count;
                    ret = current;
                }
            }
        }
        else {
            std::vector<Sudoku::DancingLinkColumn *> potentialColumns;
            for (current = (Sudoku::DancingLinkColumn *)root->right; (Sudoku::DancingLink *)current != root; current = (Sudoku::DancingLinkColumn *)current->right) {
                if (current->count < min) {
                    potentialColumns.clear();
                    potentialColumns.push_back(current);
                    min = current->count;
                    if (min == 0) return current;
                }
                else if (current->count == min) {
                    potentialColumns.push_back(current);
                }
            }
            if (potentialColumns.size() == 1) return potentialColumns[0];
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, potentialColumns.size() - 1);
            ret = potentialColumns[distrib(gen)];
        }
        return ret;
    }
};