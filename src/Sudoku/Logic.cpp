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
        auto found = Sudoku::containsLinkEqual(*i, puzzle.constraintTable->solution.begin(), puzzle.constraintTable->solution.end());
        if (found == puzzle.constraintTable->solution.end()) {
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
    }
    else if (foundSingle(&puzzle.constraintTable->root, move)) {
        foundMove = true;
    }
    if (foundMove) return move;

    for (auto i = 2; i <= 4; i++) {
        if (foundBasicMove(&puzzle.constraintTable->root, i, move)) {
            foundMove = true;
            break;
        }
    }
    return move;
}

bool Sudoku::logic::foundSingle(DancingLink *root, Move &move) {
    for (auto col = root->right; col != root; col = col->right) {
        // Should find naked singles before hidden singles
        if (col->count != 1) continue;

        move.diff = eBeginner;
        move.type = eLogicInsert;
        move.truths.push_back(col->down);
        return true;
    }
    return false;
}

bool Sudoku::logic::foundBasicMove(DancingLink *root, int num_of_candidates, Move &move) {
    for (auto truth = root->right; truth != root; truth = truth->right) {
        if (truth->count != num_of_candidates) continue;
        
        std::vector<DancingLink *> candidates;
        std::vector<DancingLinkColumn *>links;
        for (auto cand = truth->down; cand != truth; cand = cand->down) {
            candidates.push_back(cand);
        }
        links.push_back(truth->colHeader);
        for (auto i = 0; i < eConstraintTypes; i++) {
            for (auto &cand : candidates) {
                cand = cand->right; // Move all one constraint right
            }
            DancingLinkColumn *firstCol = candidates[0]->colHeader;
            bool isSameColumns = true;
            for (auto cand_itr = candidates.begin() + 1; cand_itr < candidates.end(); cand_itr++) {
                if ((*cand_itr)->colHeader != firstCol) {
                    isSameColumns = false;
                    break;
                }
            }
            if (isSameColumns) {
                links.push_back(firstCol);
            }
        }
        if (links.size() < num_of_candidates) {
            continue; // No moves found for this column
        }

        // should find naked first since moving right in constraint table
        if (links[0]->constraintType == eConstraintCell) { // is naked
            move.diff = (Sudoku::difficulty)(num_of_candidates - 2);
        }
        else { // is hidden
            move.diff = (Sudoku::difficulty)(num_of_candidates - 1);
        }
        move.type = eLogicPencil;
        move.truths = candidates;

        for (auto i = links.begin() + 1; i < links.end(); i++) { // start at +1 since links[0] are the truths
            for (auto row = (*i)->down; row != (*i); row = row->down) {
                auto found = containsLinkEqual(row, move.truths.begin(), move.truths.end());
                if (found != move.truths.end()) {
                    continue; // actually a truth
                }
                found = containsLinkEqual(row, move.falses.begin(), move.falses.end());
                if (found == move.falses.end()) { // not already in false
                    move.falses.push_back(row);
                }
            }
        }
        if (!move.falses.empty()) {
            return true;
        }
        else { // Didn't find any to remove
            move.truths.clear();
        }
    }
    return false;
}
