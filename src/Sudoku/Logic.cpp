#include "Logic.h"
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

/**
 * @brief vector containing all the methods used to find wrong moves
 * 
 */
static std::vector<bool (*)(const Sudoku::SudokuPuzzle &puzzle, std::vector<Sudoku::logic::LogicalMove> &move)> mistakes = {
    &Sudoku::logic::foundIllogicalInput,
    &Sudoku::logic::foundIllogicalPencil,
    &Sudoku::logic::foundWrongRemovedPencil,
    &Sudoku::logic::foundMissingPencilMark,
    &Sudoku::logic::foundWrongInput,
};

bool Sudoku::logic::foundIllogicalInput(const SudokuPuzzle &puzzle, std::vector<LogicalMove> &moves) {
    for (const auto &i : puzzle.wrong_inputs) {
        LogicalMove move;
        move.type = eLogicErrorInsert;
        move.diff = eBeginner;
        move.falses.push_back(i);
        moves.push_back(move);
    }
    return puzzle.wrong_inputs.size() != 0;
}

bool Sudoku::logic::foundIllogicalPencil(const SudokuPuzzle &puzzle, std::vector<LogicalMove> &moves) {
    for (const auto &i : puzzle.wrong_marks) {
        LogicalMove move;
        move.type = eLogicErrorPencil;
        move.diff = eBeginner;
        move.falses.push_back(i);
        moves.push_back(move);
    }
    return puzzle.wrong_marks.size() != 0;
}

bool Sudoku::logic::foundWrongRemovedPencil(const SudokuPuzzle &puzzle, std::vector<LogicalMove> &moves) {
    bool ret = false;
    for (const auto &i : puzzle.removed_marks) {
        auto found = false;
        for (auto link : puzzle.constraintTable.solution) {
            if (link->count == i->count) {
                found = true;
                break;
            }
        }
        if (found) {
            LogicalMove move;
            move.type = eLogicErrorInsert;
            move.diff = eBeginner;
            move.truths.push_back(i);
            moves.push_back(move);
            ret = true;
        }
    }
    return ret;
}

bool Sudoku::logic::foundMissingPencilMark(const SudokuPuzzle &puzzle, std::vector<LogicalMove> &moves) {
    bool ret = false;
    for (auto col = puzzle.constraintTable.root->right; col != puzzle.constraintTable.root.get(); col = col->right) {
        for (auto row = col->down; row != col; row = row->down) {
            bool foundInMarks = false;
            for (auto i = puzzle.pencilMarks.begin(); i < puzzle.pencilMarks.end(); i++) {
                if (row->count == (*i)->count) {
                    foundInMarks = true;
                    break;
                }
            }
            if (!foundInMarks) {
                LogicalMove move;
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

bool Sudoku::logic::foundWrongInput(const SudokuPuzzle &puzzle, std::vector<LogicalMove> &moves) {
    bool ret = false;
    for (auto i = puzzle.constraintTable.current.begin() + puzzle.current_start_index; i < puzzle.constraintTable.current.end(); i++) {
        auto found = false;
        for (const auto &link : puzzle.constraintTable.solution) {
            if (link->count == (*i)->count) {
                found = true;
                break;
            }
        }
        if (!found) { // not found in solution
            LogicalMove move;
            move.type = eLogicErrorInsert;
            move.diff = eBeginner;
            move.falses.push_back(*i);
            moves.push_back(move);
            ret = true;
        }
    }
    return ret;
}

std::vector<Sudoku::logic::LogicalMove> Sudoku::logic::foundMistake(const SudokuPuzzle &puzzle) {
    std::vector<Sudoku::logic::LogicalMove> moves;
    for (auto &method : mistakes) {
        method(puzzle, moves);
    }
    return moves;
}

std::vector<Sudoku::logic::LogicalMove> Sudoku::logic::getNextMove(const Sudoku::SudokuPuzzle &puzzle, bool ignore_mistakes) {
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

    auto columnCounts = getSortedConstraintColumns(puzzle.constraintTable.root.get());
    std::vector<LogicalMove> moves;
    foundSingle(columnCounts[eSingle], moves);
    for (int candidates = eDouble; candidates <= eTriple; candidates++) {
        foundLockedCandidates(columnCounts[candidates], moves);
    }
    foundPairs(puzzle.constraintTable.colHeaders, columnCounts[eDouble], moves);

    std::sort(moves.begin(), moves.end(), [](const LogicalMove &left, const LogicalMove &right) -> bool {
        if (left.diff == right.diff) {
            return left.truths[0]->colHeader->constraintType < right.truths[0]->colHeader->constraintType;
        }
        return left.diff < right.diff;
    });
    return moves;
}

bool Sudoku::logic::foundSingle(const DancingLinkContainer &columns, std::vector<LogicalMove> &moves) {
    bool ret = false;
    for (auto *link : columns) {
        if (link->count != 1) continue;
        LogicalMove move;
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
 *        matching links are constraint columns that contain all the rows in candidates
 *
 * @param candidates vector of candidates all in the same constraint column
 * @return std::vector<Sudoku::DancingLinkColumn *> vector of all columns
 */
static std::vector<Sudoku::DancingLinkColumn *> getMatchingLinks(Sudoku::DancingLinkContainer &candidates) {
    std::vector<Sudoku::DancingLinkColumn *> ret;
    int count = candidates.size();

    for (auto curr_col = candidates[0]->right; curr_col != candidates[0]; curr_col = curr_col->right) {
        int this_count = 1; // start at one since we aren't counting curr_col
        for (auto curr_row = curr_col->down; curr_row != curr_col; curr_row = curr_row->down) {
            if (curr_row == curr_row->colHeader) continue;
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

bool Sudoku::logic::foundLockedCandidates(const DancingLinkContainer &columns, std::vector<LogicalMove> &moves) {
    bool ret = false;
    for (auto *col : columns) {
        if (col->count < 2 || col->count > 3) continue;

        DancingLinkContainer candidates;
        candidates.reserve(col->count);
        for (auto row = col->down; row != col; row = row->down) {
            candidates.push_back(row);
        }
        std::vector<DancingLinkColumn *>links = getMatchingLinks(candidates);

        if (links.size() == 0) { // Didnt find any matching links
            continue;
        }

        LogicalMove move;
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

bool Sudoku::logic::foundPairs(const std::shared_ptr<Sudoku::DancingLinkColumn[]> &colHeaders, const DancingLinkContainer &columns, std::vector<LogicalMove> &moves) {
    bool ret = false;
    for (auto col_itr = columns.begin(); col_itr != columns.end() - 1; col_itr++) {
        auto column = *col_itr;
        std::vector<DancingLinkContainer> candidates;
        for (auto cand = column->down; cand != column; cand = cand->down) {
            candidates.push_back({cand});
        }

        for (auto inner_itr = col_itr + 1; inner_itr != columns.end(); inner_itr++) {
            // Clearing candidates from previous iterations
            auto inner_column = *inner_itr;
            for (auto vec : candidates) {
                if (vec.size() == 1) continue;
                for (auto i = vec.begin() + 1, end = vec.end(); i < end; ) {
                    i = vec.erase(i);
                    end = vec.end();
                }
            }

            auto cand_idx = 0;
            for (auto cand = inner_column->down; cand != inner_column; cand = cand->down) {
                candidates[cand_idx++].push_back(cand);
            }

            // Candidates now contains vectors of rows
            // if every vector has rows that share at least one constraint
            // then for every shared constraint remove all but those in candidates

            auto count = candidates.size();
            auto counter = 0; // Since we don't count the one we start on
            DancingLinkContainer intersection;
            DancingLinkContainer falses;
            for (auto cand_vec : candidates) {
                std::vector<DancingLinkContainer> seen; // holds {{seen by first cand}, {seen by second cand}}
                auto seen_idx = 0;
                for (auto cand : cand_vec) {
                    seen.push_back({cand->colHeader});
                    for (auto constraint = cand->right; constraint != cand; constraint = constraint->right) {
                        seen[seen_idx].push_back(constraint->colHeader);
                    }
                    seen_idx++;
                }

                // Need to go through each vector and find the intersection of all
                intersection = seen[0];
                std::sort(intersection.begin(), intersection.end());
                for (auto seen_itr = seen.begin() + 1; seen_itr != seen.end(); seen_itr++) {
                    DancingLinkContainer temp;
                    std::sort((*seen_itr).begin(), (*seen_itr).end());
                    std::set_intersection(std::begin(intersection), std::end(intersection), std::begin(*seen_itr), std::end(*seen_itr), std::back_inserter(temp));
                    intersection = temp;
                    if (intersection.size() == 0) break;
                }
                if (intersection.size() == 0) break;
                counter++;
                falses.insert(falses.end(), intersection.begin(), intersection.end());
            }
            if (falses.size() == 0) continue;
            if (count != counter) continue;

            LogicalMove move;
            move.diff = eBeginner; // Just for testing
            move.type = eLogicPencil;
            for (auto vec : candidates) {
                for (auto link : vec) {
                    move.truths.push_back(link);
                }
            }
            for (auto f : falses) {
                for (auto link = f->down; link != f; link = link->down) {
                    auto found = containsLinkEqual(link, move.truths.begin(), move.truths.end());
                    if (found != move.truths.end()) continue;
                    found = containsLinkEqual(link, move.falses.begin(), move.falses.end());
                    if (found == move.falses.end()) {
                        move.falses.push_back(link);
                    }
                }
            }
            if (move.falses.size() == 0) continue;
            moves.push_back(move);
            break;
        }
    }
    return ret;
}

auto Sudoku::logic::getSortedConstraintColumns(DancingLink *root) -> std::array<DancingLinkContainer, eSize> {
    std::array<DancingLinkContainer, eSize> ret;
    for (auto *col = root->right; col != root; col = col->right) {
        ret[(col->count - 1)].push_back(col);
    }
    return ret;
}