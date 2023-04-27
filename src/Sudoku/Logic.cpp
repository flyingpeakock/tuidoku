#include "Logic.h"
#include <algorithm>

/**
 * @brief vector containing all the methods used to find wrong moves
 * 
 */
static std::vector<bool (*)(const Sudoku::SudokuPuzzle &puzzle, std::vector<Sudoku::logic::Move> &move)> mistakes = {
    &Sudoku::logic::foundIllogicalInput,
    &Sudoku::logic::foundIllogicalPencil,
    &Sudoku::logic::foundWrongRemovedPencil,
    &Sudoku::logic::foundMissingPencilMark,
    &Sudoku::logic::foundWrongInput,
};

bool Sudoku::logic::foundIllogicalInput(const SudokuPuzzle &puzzle, std::vector<Move> &moves) {
    for (const auto &i : puzzle.wrong_inputs) {
        Move move;
        move.type = eLogicErrorInsert;
        move.diff = eBeginner;
        move.falses.push_back(i);
        moves.push_back(move);
    }
    return puzzle.wrong_inputs.size() != 0;
}

bool Sudoku::logic::foundIllogicalPencil(const SudokuPuzzle &puzzle, std::vector<Move> &moves) {
    for (const auto &i : puzzle.wrong_marks) {
        Move move;
        move.type = eLogicErrorPencil;
        move.diff = eBeginner;
        move.falses.push_back(i);
        moves.push_back(move);
    }
    return puzzle.wrong_marks.size() != 0;
}

bool Sudoku::logic::foundWrongRemovedPencil(const SudokuPuzzle &puzzle, std::vector<Move> &moves) {
    bool ret = false;
    for (const auto &i : puzzle.removed_marks) {
        auto found = containsLinkEqual(getRowFromLink(i), getColFromLink(i), getNumFromLink(i), puzzle.constraintTable->solution.begin(), puzzle.constraintTable->solution.end());
        if (found != puzzle.constraintTable->solution.end()) {
            Move move;
            move.type = eLogicErrorInsert;
            move.diff = eBeginner;
            move.truths.push_back(i);
            moves.push_back(move);
            ret = true;
        }
    }
    return ret;
}

bool Sudoku::logic::foundMissingPencilMark(const SudokuPuzzle &puzzle, std::vector<Move> &moves) {
    bool ret = false;
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
                Move move;
                move.type = eLogicErrorPencilMissing;
                move.diff = eBeginner;
                move.truths.push_back(row);
                moves.push_back(move);
                ret = true;
            }

        }
    }
    return ret;
}

bool Sudoku::logic::foundWrongInput(const SudokuPuzzle &puzzle, std::vector<Move> &moves) {
    bool ret = false;
    for (auto i = puzzle.constraintTable->current.begin() + puzzle.current_start_index; i < puzzle.constraintTable->current.end(); i++) {
        auto found = Sudoku::containsLinkEqual(*i, puzzle.constraintTable->solution.begin(), puzzle.constraintTable->solution.end());
        if (found == puzzle.constraintTable->solution.end()) {
            Move move;
            move.type = eLogicErrorInsert;
            move.diff = eBeginner;
            move.falses.push_back(*i);
            moves.push_back(move);
            ret = true;
        }
    }
    return ret;
}

std::vector<Sudoku::logic::Move> Sudoku::logic::foundMistake(const SudokuPuzzle &puzzle) {
    std::vector<Sudoku::logic::Move> moves;
    for (auto &method : mistakes) {
        method(puzzle, moves);
    }
    return moves;
}

std::vector<Sudoku::logic::Move> Sudoku::logic::getNextMove(const Sudoku::SudokuPuzzle &puzzle, bool ignore_mistakes) {
    enum {
        eSingle,
        eDouble,
        eTriple,
        eQuadruple,
    };

    if (!ignore_mistakes) {
        auto mistakes = foundMistake(puzzle);
        if (mistakes.size() != 0) {
            return mistakes;
        }
    }

    auto columnCounts = getSortedConstraintColumns(puzzle.constraintTable->root.get());
    std::vector<Move> moves;
    foundSingle(columnCounts[eSingle], moves);
    for (int candidates = eSingle; candidates <= eTriple; candidates++) {
        foundLockedCandidates(columnCounts[candidates], moves);
    }

    std::sort(moves.begin(), moves.end(), [](const Move &left, const Move &right) -> bool {
        return left.diff < right.diff;
    });
    return moves;
}

bool Sudoku::logic::foundSingle(const std::vector<DancingLink *> &columns, std::vector<Move> &moves) {
    bool ret = false;
    for (auto *link : columns) {
        if (link->count != 1) continue;
        Move move;
        move.diff = eBeginner;
        move.type = eLogicInsert;
        move.truths.push_back(link->down);
        moves.push_back(move);
        ret = true;

    }
    return ret;
}

/**
 * @brief Get the matching links that are uncovered and have the same candidates
 *
 * @param candidates vector of candidates all in the same constraint column
 * @return std::vector<Sudoku::DancingLinkColumn *> vector of all columns
 */
static std::vector<Sudoku::DancingLinkColumn *> getMatchingLinks(std::vector<Sudoku::DancingLink *> candidates) {
    std::vector<Sudoku::DancingLinkColumn *> ret;
    int count = candidates.size();

    for (auto curr_col = candidates[0]->right; curr_col != candidates[0]; curr_col = curr_col->right) {
        int this_count = 1; // start at one since we aren't counting curr_col
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

bool Sudoku::logic::foundLockedCandidates(const std::vector<DancingLink *> &columns, std::vector<Move> &moves) {
    bool ret = false;
    for (auto *col : columns) {
        if (col->count < 2 || col->count > 3) continue;

        std::vector<DancingLink *> candidates;
        candidates.reserve(col->count);
        for (auto row = col->down; row != col; row = row->down) {
            candidates.push_back(row);
        }
        std::vector<DancingLinkColumn *>links = getMatchingLinks(candidates);

        if (links.size() == 0) { // Didnt find any matching links
            continue;
        }

        Move move;
        move.type = eLogicPencil; // Found pencil marks to remove
        move.diff = (difficulty)(col->count - 1);
        move.truths = candidates;

        for (const auto &link : links) {
            for (auto *row = link->down; row != link; row = row->down) {
                auto found = containsLinkEqual(row, move.truths.begin(), move.truths.end());
                if (found != move.truths.end()) {
                    // is actually a truth
                    continue;
                }
                found = containsLinkEqual(row, move.falses.begin(), move.falses.end());
                if (found != move.falses.end()) {
                    // Already added
                    continue;
                }
                move.falses.push_back(row);
            }
        }
        if (move.falses.size() != 0) {
            moves.push_back(move);
            ret = true;
        }
    }

    return ret;
}

auto Sudoku::logic::getSortedConstraintColumns(DancingLink *root) -> std::array<std::vector<DancingLink *>, eSize> {
    std::array<std::vector<DancingLink *>, eSize> ret;
    for (auto *col = root->right; col != root; col = col->right) {
        ret[(col->count - 1)].push_back(col);
    }
    return ret;
}