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
    for (auto col = puzzle.constraintTable->root->right; col != puzzle.constraintTable->root.get(); col = col->right) {
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
    else if (foundSingle(puzzle.constraintTable->root.get(), move)) {
        foundMove = true;
    }
    if (foundMove) return move;

    for (auto i = 2; i <= 4; i++) {
        if (foundBasicMove(puzzle.constraintTable->root.get(), i, move)) {
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

/**
 * @brief Get the matching links that are uncovered and have the same candidates
 *
 * @param candidates vector of candidates all in the same constraint column
 * @return std::vector<Sudoku::DancingLinkColumn *> vector of all columns
 */
static std::vector<Sudoku::DancingLinkColumn *> getMatchingLinks(std::vector<Sudoku::DancingLink *> candidates) {
    std::vector<Sudoku::DancingLinkColumn *> ret;
    ret.push_back(candidates[0]->colHeader);
    int count = candidates.size();

    for (auto curr_col = candidates[0]->right; curr_col != candidates[0]; curr_col = curr_col->right) {
        int this_count = 1; // start at one since we are counting curr_col
        for (auto curr_row = curr_col->down; curr_row != curr_col; curr_row = curr_row->down) {
            auto found = Sudoku::containsLinkEqual(curr_row, candidates.begin() + 1, candidates.end());
            if (found != candidates.end()) {
                this_count++;
            }
        }
        if (this_count == count) {
            ret.push_back(curr_col->colHeader);
        }
    }
    return ret;
}

bool Sudoku::logic::foundBasicMove(DancingLink *root, int num_of_candidates, Move &move) {
    for (auto truth = root->right; truth != root; truth = truth->right) {
        if (truth->count != num_of_candidates) continue;
        
        std::vector<DancingLink *> candidates;
        for (auto cand = truth->down; cand != truth; cand = cand->down) {
            candidates.push_back(cand);
        }
        std::vector<DancingLinkColumn *>links = getMatchingLinks(candidates);

        if (links.size() < num_of_candidates) {
            continue; // No moves found for this column
        }

        // should find naked first since moving right in constraint table
        if (links[0]->constraintType == eConstraintCell) { // is naked
            move.diff = (Sudoku::difficulty)(num_of_candidates - 1);
        }
        else { // is hidden
            move.diff = (Sudoku::difficulty)(num_of_candidates);
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
