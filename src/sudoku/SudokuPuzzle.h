#ifndef SUDOKU_PUZZLE_H
#define SUDOKU_PUZZLE_H

#include "DancingLink.h"
#include "Constants.h"

namespace sudoku {

    struct Move {
        enum moveType {
            eCoverFull,
            eCoverRow
        };
        moveType type;
        DancingLink *link;
    };

    struct SudokuPuzzle {
        SudokuPuzzle(sudoku::DancingLinkTable &table);
        sudoku::DancingLinkTable constraintTable;
        int current_start_index;
        sudoku::DancingLinkContainer wrong_inputs;
        sudoku::DancingLinkContainer pencilMarks;   // contains visible marks
        sudoku::DancingLinkContainer wrong_marks;   // contains visible marks that are wrong
        sudoku::DancingLinkContainer removed_marks; // contains marks that have been visible but no longer are
        std::vector<Move> moves;
        logic::LogicalMove nextMove;

        void insert(int row, int col, char num);
        void pencil(int row, int col, char num);
        void autoPencil();
        void getNextMove();

        void recheckMistakes(sudoku::DancingLink *link);
    };
}

#endif // SUDOKU_PUZZLE_H