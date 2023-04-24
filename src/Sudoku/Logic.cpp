#include "Logic.h"

/**
 * @brief vector containing all the methods used to find wrong moves
 * 
 */
static std::vector<bool (*)(const Sudoku::SudokuPuzzle &puzzle, Sudoku::logic::Move &move)> mistakes = {
    &Sudoku::logic::foundIllogicalInput,
    &Sudoku::logic::foundIllogicalPencil,
    &Sudoku::logic::foundWrongRemovedPencil,
    &Sudoku::logic::foundMissingPencilMark,
    &Sudoku::logic::foundWrongInput,
};

bool Sudoku::logic::foundIllogicalInput(const SudokuPuzzle &puzzle, Move &move) {
    for (const auto &i : puzzle.wrong_inputs) {
        move.type = eLogicErrorInsert;
        move.diff = eBeginner;
        move.falses.push_back(i);
        return true;
    }
    return false;
}

bool Sudoku::logic::foundIllogicalPencil(const SudokuPuzzle &puzzle, Move &move) {
    for (const auto &i : puzzle.wrong_marks) {
        move.type = eLogicErrorPencil;
        move.diff = eBeginner;
        move.falses.push_back(i);
        return true;
    }
    return false;
}

bool Sudoku::logic::foundWrongRemovedPencil(const SudokuPuzzle &puzzle, Move &move) {
    for (const auto &i : puzzle.removed_marks) {
        auto found = containsLinkEqual(getRowFromLink(i), getColFromLink(i), getNumFromLink(i), puzzle.constraintTable->solution.begin(), puzzle.constraintTable->solution.end());
        if (found != puzzle.constraintTable->solution.end()) {
            move.type = eLogicErrorInsert;
            move.diff = eBeginner;
            move.truths.push_back(i);
            return true;
        }
    }
    return false;
}

bool Sudoku::logic::foundMissingPencilMark(const SudokuPuzzle &puzzle, Move &move) {
    for (auto col = puzzle.constraintTable->root.right; col != &puzzle.constraintTable->root; col = col->right) {
        for (auto row = col->down; row != col; row = row->down) {
            bool foundInMarks = false;
            for (auto i = puzzle.pencilMarks.begin(); i < puzzle.pencilMarks.end(); i++) {
                if (row->count == (*i)->count) {
                    foundInMarks = true;
                    break;
                }
            }
            if (!foundInMarks) {
                move.type = eLogicErrorPencilMissing;
                move.diff = eBeginner;
                move.truths.push_back(row);
                return true;
            }

        }
    }
    return false;
}

bool Sudoku::logic::foundWrongInput(const SudokuPuzzle &puzzle, Move &move) {
    for (auto i = puzzle.constraintTable->current.begin() + puzzle.current_start_index; i < puzzle.constraintTable->current.end(); i++) {
        bool isInSolution = false;
        for (auto j = puzzle.constraintTable->solution.begin(); j < puzzle.constraintTable->solution.end(); j++) {
            if ((*j)->count == (*i)->count) {
                isInSolution = true;
                break;
            }
        }
        if (!isInSolution) {
            move.type = eLogicErrorInsert;
            move.diff = eBeginner;
            move.falses.push_back(*i);
            return true;
        }
    }
    return false;
}

bool Sudoku::logic::foundMistake(const SudokuPuzzle &puzzle, Move &move) {
    for (auto &method : mistakes) {
        if (method(puzzle, move)) {
            return true;
        }
    }
    return false;
}

Sudoku::logic::Move Sudoku::logic::getNextMove(const Sudoku::SudokuPuzzle &puzzle, bool &foundMove) {
    Move move;
    foundMove = false;
    if (foundMistake(puzzle, move)) {
        foundMove = true;
        return move;
    }
    return move;
}