#include "Logic.h"

#include <algorithm>      // for sort, set_intersection, unique
#include <iterator>       // for back_insert_iterator, begin, end, back_inse...
#include <compare>        // for operator<, __synth3way_t
#include <memory>         // for shared_ptr, __shared_ptr_access
#include <vector>         // for vector

#include "DancingLink.h"  // for containsLinkEqual, getColFromLink, getRowFr...

/**
 * @brief vector containing all the methods used to find wrong moves
 * 
 */
static std::vector<bool (*)(const sudoku::SudokuPuzzle &puzzle, sudoku::logic::LogicalMove &move)> mistakes = {
    &sudoku::logic::foundIllogicalInput,
    &sudoku::logic::foundIllogicalPencil,
    &sudoku::logic::foundWrongRemovedPencil,
    &sudoku::logic::foundMissingPencilMark,
    &sudoku::logic::foundWrongInput,
};

bool sudoku::logic::foundIllogicalInput(const SudokuPuzzle &puzzle, LogicalMove &move) {
    if (puzzle.wrong_inputs.size() == 0) return false;
    move.type = eLogicErrorInsert;
    move.diff = eBeginner;
    move.falses.push_back(puzzle.wrong_inputs[0]);
    return true;
}

bool sudoku::logic::foundIllogicalPencil(const SudokuPuzzle &puzzle, LogicalMove &move) {
    if (puzzle.wrong_marks.size() == 0) return false;
    move.type = eLogicErrorPencil;
    move.diff = eBeginner;
    move.falses.push_back(puzzle.wrong_marks[0]);
    return true;
}

bool sudoku::logic::foundWrongRemovedPencil(const SudokuPuzzle &puzzle, LogicalMove &move) {
    for (const auto &i : puzzle.removed_marks) {
        auto found = false;
        for (auto link : puzzle.constraintTable.solution) {
            if (link->count == i->count) {
                found = true;
                break;
            }
        }
        if (found) {
            move.type = eLogicErrorInsert;
            move.diff = eBeginner;
            move.truths.push_back(i);
            return true;
        }
    }
    return false;
}

bool sudoku::logic::foundMissingPencilMark(const SudokuPuzzle &puzzle, LogicalMove &move) {
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
                move.type = eLogicErrorPencilMissing;
                move.diff = eBeginner;
                move.truths.push_back(row);
                return true;
            }

        }
    }
    return false;
}

bool sudoku::logic::foundWrongInput(const SudokuPuzzle &puzzle, LogicalMove &move) {
    for (auto i = puzzle.constraintTable.current.begin() + puzzle.current_start_index; i < puzzle.constraintTable.current.end(); i++) {
        auto found = false;
        for (const auto &link : puzzle.constraintTable.solution) {
            if (link->count == (*i)->count) {
                found = true;
                break;
            }
        }
        if (!found) { // not found in solution
            move.type = eLogicErrorInsert;
            move.diff = eBeginner;
            move.falses.push_back(*i);
            return true;
        }
    }
    return false;
}

sudoku::logic::LogicalMove sudoku::logic::foundMistake(const SudokuPuzzle &puzzle) {
    sudoku::logic::LogicalMove move;
    move.type = eMoveNotFound;
    for (auto &method : mistakes) {
        if (method(puzzle, move)) {
            return move;
        }
    }
    return move;
}

static int cumm_count(const sudoku::DancingLink *col) {
    int ret = 0;
    for (auto row = col->down; row != col; row = row->down) {
        for (auto next = row->right; next != row; next = next->right) {
            ret += next->colHeader->count;
        }
    }
    return ret;
}

sudoku::logic::LogicalMove sudoku::logic::getNextMove(const sudoku::SudokuPuzzle &puzzle, bool ignore_mistakes) {
    if (!ignore_mistakes) {
        auto mistakes = foundMistake(puzzle);
        if (mistakes.type != eMoveNotFound) {
            return mistakes;
        }
    }

    DancingLinkContainer columns;
    for (auto col = puzzle.constraintTable.root->right; col != puzzle.constraintTable.root.get(); col = col->right) {
        columns.push_back(col);
    }
    std::sort(columns.begin(), columns.end(), [](const DancingLink *l, const DancingLink *r){
        if (l->count == r->count) {
            return cumm_count(l) > cumm_count(r);
        }
        else {
            return l->count < r->count;
        }
    });

    for (const auto &col : columns) {
        LogicalMove move;
        if (xlogic(col, move)) {
            return move;
        }
    }
    LogicalMove move;
    move.type = eMoveNotFound;
    return move;
}

bool sudoku::logic::foundSingle(const DancingLink *column, LogicalMove &move) {
    move.diff = eBeginner;
    move.type = eLogicInsert;
    move.truths.push_back(column->down);
    return true;
}

/**
 * @brief Get the matching links that are uncovered and have the same candidates
 *        matching links are constraint columns that contain all the rows in candidates
 *
 * @param candidates vector of candidates all in the same constraint column
 * @return std::vector<sudoku::DancingLinkColumn *> vector of all columns
 */
static sudoku::DancingLinkContainer getMatchingLinks(sudoku::DancingLinkContainer &candidates) {
    sudoku::DancingLinkContainer ret;
    int count = candidates.size();

    for (auto curr_col = candidates[0]->right; curr_col != candidates[0]; curr_col = curr_col->right) {
        int this_count = 1; // start at one since we aren't counting curr_col
        for (auto curr_row = curr_col->down; curr_row != curr_col; curr_row = curr_row->down) {
            if (curr_row == curr_row->colHeader) continue;
            auto found = sudoku::containsLinkEqual(curr_row, candidates.begin() + 1, candidates.end());
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

bool sudoku::logic::foundLockedCandidates(const DancingLink *column, const DancingLinkContainer &candidates, const DancingLinkContainer &links, LogicalMove &move) {
    bool ret = false;

    move.type = eLogicPencil; // Found pencil marks to remove
    move.diff = (difficulty)(column->count - 1);
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
        ret = true;
    }

    return ret;
}

static bool foundLinks(sudoku::DancingLinkContainer &candidates,
                      std::vector<sudoku::DancingLinkContainer> &intersections,
                      const sudoku::DancingLinkContainer::const_iterator &begin,
                      const sudoku::DancingLinkContainer::const_iterator &end,
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
        std::vector<sudoku::DancingLinkContainer> seen;
        auto seen_idx = 0;
        bool invalid_column = false;
        for (auto cand = column->down; cand != column; cand = cand->down) {
            auto found = sudoku::containsLinkEqual(cand, candidates.begin(), candidates.end());
            if (found != candidates.end()) {
                invalid_column = true;
                break;
            }
            candidates.push_back(cand);
            seen.push_back(sudoku::DancingLinkContainer());
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
        std::vector<sudoku::DancingLinkContainer> new_intersections;
        for (auto i = copy.begin(); i < copy.end(); i++) {
            sudoku::DancingLinkContainer new_intersections_inner = *i;
            for (auto j = seen.begin(); j < seen.end(); j++) {
                sudoku::DancingLinkContainer temp; // An intersection between a row in this column and in previous column
                std::set_intersection(std::begin(new_intersections_inner),
                                      std::end(new_intersections_inner),
                                      std::begin(*j),
                                      std::end(*j),
                                      std::back_inserter(temp));
                if (temp.size() != 0) {
                    seen.erase(j);
                    new_intersections_inner = temp;
                    break;
                }
            }
            if (new_intersections_inner.size() == 0) {
                break;
            }
            new_intersections.push_back(new_intersections_inner);
        }
        if (seen.size() != 0) continue;
        if (new_intersections.size() < count) continue;

        if ((depth + 1) == count) {
            intersections = new_intersections;
            return true;
        }
        else if (foundLinks(candidates, new_intersections, col_itr + 1, end + 1, depth + 1, count)) {
            intersections = new_intersections;
            return true;
        }
    }
    candidates.pop_back();
    return false;
}

void getAllSeenHelper(sudoku::DancingLinkContainer &vec, const sudoku::DancingLink *column, int count, int depth) {
    for (auto row = column->down; row != column; row = row->down) {
        if (row->colHeader == row) continue;
        for (auto col = row->right; col != row; col = col->right) {
            if (col->colHeader->count <= count) {
                vec.push_back(col->colHeader);
            }
            if (depth < count) {
                getAllSeenHelper(vec, col, count, depth + 1);
            }
        }
    }
}

sudoku::DancingLinkContainer getAllSeen(const sudoku::DancingLinkColumn *column, int count) {
    sudoku::DancingLinkContainer ret;
    getAllSeenHelper(ret, column, count, 1);
    if (ret.size() > 1) {
        std::sort(ret.begin(), ret.end());
        auto last = std::unique(ret.begin(), ret.end());
        ret.erase(last, ret.end());
    }
    return ret;
}

bool sudoku::logic::xlogic(const DancingLink *column, LogicalMove &move) {
    auto count = column->count;

    if (count > 4) return false;

    if (count == 1) {
        return foundSingle(column, move);
    }

    DancingLinkContainer candidates;
    candidates.reserve(count);
    for (auto row = column->down; row != column; row = row->down) {
        candidates.push_back(row);
    }

    if (count <= 3) {
        auto hard_links = getMatchingLinks(candidates);
        if (hard_links.size() != 0) {
            if (foundLockedCandidates(column, candidates, hard_links, move)) {
                return true;
            }
        }
    }

    std::vector<DancingLinkContainer> intersections;
    auto intersect_idx = 0;
    for (auto cand = column->down; cand != column; cand = cand->down) {
        intersections.push_back(DancingLinkContainer());
        for (auto col = cand->right; col != cand; col = col->right) {
            intersections[intersect_idx].push_back(col->colHeader);
        }
        intersect_idx++;
    }
    for (auto &intersect : intersections) {
        std::sort(intersect.begin(), intersect.end());
    }


    auto seen_columns = getAllSeen(column->colHeader, count);
    if (foundLinks(candidates, intersections, seen_columns.begin(), seen_columns.end() - (count - 2), 1, count)) {
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
                    if (found != move.falses.end()) continue;
                    move.falses.push_back(row);
                }
            }
        }
        if (move.falses.size() == 0) return false;

        if (move.truths.front()->colHeader->constraintType == eConstraintCell) {
            move.diff = (difficulty)(count - 1);
        }
        else {
            if (((move.truths.front()->colHeader->constraintType == eConstraintRow)
                    || (move.truths.front()->colHeader->constraintType == eConstraintCol))
                && ((move.truths.back()->colHeader->constraintType == eConstraintRow)
                    || (move.truths.back()->colHeader->constraintType == eConstraintCol)))
            {
                auto func = &sudoku::getColFromLink;
                if (move.truths.front()->colHeader->constraintType == eConstraintRow) {
                    func = &sudoku::getRowFromLink;
                }

                if (func(move.truths.front()) != func(move.truths.back())) {
                    move.diff = (difficulty)(eHard + (count - 2));
                }
            }
        }
        if ((int)move.diff > (int)eHighestDifficulty) {
            move.diff = eHighestDifficulty;
        }
        return true;
    }
    return false;
}
