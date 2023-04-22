#pragma once
#include "Constants.h"
#include "DancingLink.h"

namespace Sudoku {
    /**
     * @brief solves a sudoku puzzle
     * 
     * @param table .. puzzle represented as a constraint table
     *                 the table should be fully created
     * @param randomize .. randomize for creating puzzles 
     * @return true .. if a unique solution
     * @return false .. if none or multiple solutions
     */
    bool solve(DancingLinkTable *table, bool randomize);

    /**
     * @brief Generate a sudoku puzzle with a unique solution
     * 
     * @return DancingLinkTable 
     */
    DancingLinkTable generate();

    struct SudokuPuzzle {
        SudokuPuzzle(Sudoku::DancingLinkTable *table);
        Sudoku::DancingLinkTable *constraintTable;
        int current_start_index;
        std::vector<Sudoku::DancingLink *> wrong_inputs;
        std::vector<Sudoku::DancingLink *> pencilMarks;   // contains visible marks
        std::vector<Sudoku::DancingLink *> wrong_marks;   // contains visible marks that are wrong
        std::vector<Sudoku::DancingLink *> removed_marks; // contains marks that have been visible but no longer are

        void insert(int row, int col, char num);
        void pencil(int row, int col, char num);
        void pencilAuto(int row, int col, char num);

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

};