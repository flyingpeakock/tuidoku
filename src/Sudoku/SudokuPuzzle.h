#ifndef SUDOKU_PUZZLE_H
#define SUDOKU_PUZZLE_H

#include "DancingLink.h"
#include "Constants.h"

namespace Sudoku {

    struct Move {
        enum moveType {
            eCoverFull,
            eCoverRow
        };
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
        logic::LogicalMove nextMove;

        void insert(int row, int col, char num);
        void pencil(int row, int col, char num);
        void autoPencil();
        void getNextMove();

        void recheckMistakes(Sudoku::DancingLink *link);
    };
}

#endif // SUDOKU_PUZZLE_H