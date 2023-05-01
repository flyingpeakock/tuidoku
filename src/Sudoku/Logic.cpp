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

static bool foundLinks(Sudoku::DancingLinkContainer &candidates,
                      std::vector<Sudoku::DancingLinkContainer> &intersections,
                      const Sudoku::DancingLinkContainer::const_iterator &begin,
                      const Sudoku::DancingLinkContainer::const_iterator &end,
                      int depth,
                      int count)
{
    auto size = candidates.size();
    for (auto col_itr = begin; col_itr < end; col_itr++) {
        auto column = *col_itr;

        // Candidates contains vectors of vectors of all candidates in all checked constraint columns
        while (candidates.size() > size) {
            candidates.pop_back();
        }
        std::vector<Sudoku::DancingLinkContainer> seen;
        auto seen_idx = 0;
        bool invalid_column = false;
        for (auto cand = column->down; cand != column; cand = cand->down) {
            auto found = Sudoku::containsLinkEqual(cand, candidates.begin(), candidates.end());
            if (found != candidates.end()) {
                invalid_column = true;
                break;
            }
            candidates.push_back(cand);
            seen.push_back(Sudoku::DancingLinkContainer());
            for (auto seen_con = cand->right; seen_con != cand; seen_con = seen_con->right) {
                seen[seen_idx].push_back(seen_con->colHeader);
            }
            seen_idx++;
        }
        if (invalid_column) continue;
        for (auto &s : seen) {
            std::sort(s.begin(), s.end());
        }

        auto copy = intersections;
        std::vector<Sudoku::DancingLinkContainer> new_intersections;
        for (auto i = seen.begin(); i < seen.end(); i++) {
            Sudoku::DancingLinkContainer new_intersections_inner = *i;
            for (auto j = copy.begin(); j < copy.end(); j++) {
                Sudoku::DancingLinkContainer temp; // An intersection between a row in this column and in previous column
                std::set_intersection(std::begin(new_intersections_inner),
                                      std::end(new_intersections_inner),
                                      std::begin(*j),
                                      std::end(*j),
                                      std::back_inserter(temp));
                if (temp.size() != 0) {
                    copy.erase(j);
                    new_intersections_inner = temp;
                    break;
                }
            }
            if (new_intersections_inner.size() == 0) {
                break;
            }
            new_intersections.push_back(new_intersections_inner);
        }
        if (copy.size() != 0) continue;
        if (new_intersections.size() < count) continue;

        if ((depth + 1) == count) {
            intersections = new_intersections;
            return true;
        }
        else if (foundLinks(candidates, new_intersections, col_itr + 1, end + 1, depth++, count)) {
            intersections = new_intersections;
            return true;
        }
    }
    candidates.pop_back();
    return false;
}

bool Sudoku::logic::foundPairs(const std::shared_ptr<Sudoku::DancingLinkColumn[]> &colHeaders, const DancingLinkContainer &columns, std::vector<LogicalMove> &moves) {
    bool ret = false;
    if (columns.size() == 0) return ret;
    auto count = columns[0]->count;
    auto end = columns.end() - (count - 1);
    for (auto col_itr = columns.begin(); col_itr != end; col_itr++) {
        auto column = *col_itr;
        DancingLinkContainer candidates;
        std::vector<DancingLinkContainer> intersections;

        auto intersect_idx = 0;
        for (auto cand = column->down; cand != column; cand = cand->down) {
            intersections.push_back(DancingLinkContainer());
            candidates.push_back(cand);
            for (auto col = cand->right; col != cand; col = col->right) {
                intersections[intersect_idx].push_back(col->colHeader);
            }
            intersect_idx++;
        }
        for (auto &intersect : intersections) {
            std::sort(intersect.begin(), intersect.end());
        }

        if (!foundLinks(candidates, intersections, col_itr + 1, end + 1, 1, count)) continue;

        LogicalMove move;
        move.type = eLogicPencil;
        move.diff = (difficulty)count;
        for (auto *link : candidates) {
            auto found = containsLinkEqual(link, move.truths.begin(), move.truths.end());
            if (found == move.truths.end()) {
                move.truths.push_back(link);
            }
        }

        for (auto &intersection : intersections) {
            for (auto &intersect : intersection) {
                for (auto row = intersect->down; row != intersect; row = row->down) {
                    auto found = containsLinkEqual(row, move.truths.begin(), move.truths.end());
                    if (found != move.truths.end()) continue;
                    found = containsLinkEqual(row, move.falses.begin(), move.falses.end());
                    if (found == move.falses.end()) {
                        move.falses.push_back(row);
                    }
                }
            }
        }
        if (move.falses.size() == 0) continue;

        if (move.truths.front()->colHeader->constraintType == eConstraintCell) {
            move.diff = (difficulty)(count - 1);
        }
        else {
            if (((move.truths.front()->colHeader->constraintType == eConstraintRow)
                    || (move.truths.front()->colHeader->constraintType == eConstraintCol))
                && ((move.truths.back()->colHeader->constraintType == eConstraintRow)
                    || (move.truths.back()->colHeader->constraintType == eConstraintCol)))
            {
                auto func = &Sudoku::getColFromLink;
                if (move.truths.front()->colHeader->constraintType == eConstraintRow) {
                    func = &Sudoku::getRowFromLink;
                }

                if (func(move.truths.front()) != func(move.truths.back())) {
                    move.diff = eHard;
                }
            }
        }
        
        moves.push_back(move);
        ret = true;
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