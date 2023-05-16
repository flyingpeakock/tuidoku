#include "sudoku.h"

#include <array>           // for array
#include <compare>         // for operator<, __synth3way_t
#include <memory>          // for shared_ptr, __shared_ptr_access

#include "DancingLink.h"   // for containsLinkEqual, containsLinkEquivalent
#include "Logic.h"         // for getNextMove
#include "SudokuPuzzle.h"  // for SudokuPuzzle, Move

static void uncoverInVector(std::vector<sudoku::Move> &vector, sudoku::DancingLink *link);

sudoku::SudokuPuzzle::SudokuPuzzle(sudoku::DancingLinkTable &table) :
    constraintTable(table),
    current_start_index(table.current.size()) {
}

void sudoku::SudokuPuzzle::pencil(int row, int col, char num) {
    if ((num < '1') || num > '9') {
        // invalid input
        return;
    }

    auto found = sudoku::containsLinkEquivalent(row, col, constraintTable.current.begin(), constraintTable.current.end());
    if (found != constraintTable.current.end()) {
        return; // filled in square
    }
    found = sudoku::containsLinkEquivalent(row, col, wrong_inputs.begin(), wrong_inputs.end());
    if (found != wrong_inputs.end()) {
        return; // filled in square
    }

    // Checking if it is removing a false in nextMove
    if (nextMove.type != logic::eMoveNotFound) {
        auto found = containsLinkEqual(row, col, num - '1', nextMove.falses.begin(), nextMove.falses.end());
        if (found != nextMove.falses.end()) {
            nextMove.falses.erase(found);
            if (nextMove.falses.size() == 0) {
                nextMove.type = logic::eMoveNotFound;
            }
        }
    }

    /*
     * if exists in wrong_marks, simply remove from wrong_marks, mark is being removed
     * if exists in pencilMarks and covered add to wrong_marks
     * if exists in removed_marks, uncover all rows until row backwards, then add row to removed_marks, then cover all rows from row forwards
     * if exists in pencilMarks, cover the row and add to removed_marks, mark is being removed
     * 
     * else search through constraint table
     *      if exists, add to pencilMarks
     *      if not exists, find in buffer and add to wrong_marks
     * 
     * if autopencil if exists in constraint table cover row and add to removed_marks
     * if autopencil and not exists in constraint table return
     * auto pencil should be handled in another function
     */

    found = sudoku::containsLinkEqual(row, col, num - '1', wrong_marks.begin(), wrong_marks.end());
    if (found != wrong_marks.end()) {
        wrong_marks.erase(found);
        return;
    }

    found = sudoku::containsLinkEqual(row, col, num - '1', pencilMarks.begin(), pencilMarks.end());
    if ((found != pencilMarks.end()) && (!sudoku::isUncovered(*found))) {
        wrong_marks.push_back(*found);
        return;
    }

    found = sudoku::containsLinkEqual(row, col, num - '1', removed_marks.begin(), removed_marks.end());
    if (found != removed_marks.end()) {
        // Putting back a mark that was removed
        uncoverInVector(moves, *found);
        pencilMarks.push_back(*found);
        removed_marks.erase(found);
        return;
    }

    found = sudoku::containsLinkEqual(row, col, num - '1', pencilMarks.begin(), pencilMarks.end());
    if (found != pencilMarks.end()) {
        sudoku::cover_row(*found);
        removed_marks.push_back(*found);
        moves.push_back({Move::eCoverRow, *found});
        pencilMarks.erase(found);
        return;
    }

    int constraints[eConstraintTypes];
    sudoku::calculateConstraintColumns(constraints, row, col, num - '1');
    for (auto loop = 0; loop < 2; loop++) {
        for (const auto &i : constraints) {
            auto colHeader = &constraintTable.colHeaders->at(i);
            if ((sudoku::isUncovered(colHeader)) || (loop > 0)) {
                for (auto r = colHeader->down; r != colHeader; r = r->down) {
                    if (sudoku::isLinkValues(r, row, col, num - '1')) {
                        if (loop == 0) {
                            pencilMarks.push_back(r);
                        }
                        else {
                            wrong_marks.push_back(r);
                        }
                        return;
                    }
                }
            }
        }
    }
}

void sudoku::SudokuPuzzle::insert(int row, int col, char num) {
    // Checking if position is a clue
    auto found = sudoku::containsLinkEquivalent(row, col, constraintTable.current.begin(), constraintTable.current.begin() + current_start_index);
    if (found != constraintTable.current.begin() + current_start_index) {
        return;
    }

    if (num == '0') {
        return removeFromPuzzle(this, row, col);
    }
    if ((num < '1') || num > '9') {
        // invalid input
        return;
    }

    if (nextMove.type != logic::eMoveNotFound) {
        auto found = containsLinkEqual(row, col, num - '1', nextMove.truths.begin(), nextMove.truths.end());
        if (found != nextMove.truths.end()) {
            nextMove.truths.erase(found);
            if (nextMove.truths.size() == 0) {
                nextMove.type = logic::eMoveNotFound;
            }
        }
    }

    // Checking if position already filled
    // in wrong input
    found = sudoku::containsLinkEquivalent(row, col, wrong_inputs.begin(), wrong_inputs.end());
    if (found != wrong_inputs.end()) {
        wrong_inputs.erase(found);
        recheckMistakes(*found);
        if ((num - '1') == sudoku::getNumFromLink(*found)) {
            // Removed if number already there
            return;
        }
    }
    else {
        // checking in current
        found = sudoku::containsLinkEquivalent(row, col, constraintTable.current.begin() + current_start_index, constraintTable.current.end());
        if (found != constraintTable.current.end()) {
            uncoverInVector(moves, *found);
            constraintTable.current.erase(found);
            recheckMistakes(*found);
            if ((num - '1') == sudoku::getNumFromLink(*found)) {
                // Removed if number is already there
                return;
            }
        }
    }

    // The cell at row col is now empty
    // Check if it is missing from constraint table because of removed pencil
    found = sudoku::containsLinkEqual(row, col, num - '1', removed_marks.begin(), removed_marks.end());
    if (found != removed_marks.end()) {
        uncoverInVector(moves, *found);
        removed_marks.erase(found);
    }

    // Add chosen link to current and cover
    int constraints[eConstraintTypes];
    sudoku::calculateConstraintColumns(constraints, row, col, num - '1');
    for (auto i : constraints) {
        if (sudoku::isUncovered(&constraintTable.colHeaders->at(i))) {
            for (auto link = constraintTable.colHeaders->at(i).down; link != &constraintTable.colHeaders->at(i); link = link->down) {
                if (sudoku::isLinkValues(link, row, col, num - '1')) {
                    link->colHeader->cover();
                    sudoku::cover_link(link);
                    constraintTable.current.push_back(link);
                    moves.push_back({Move::eCoverFull, link});
                    recheckMistakes(link);
                    return;
                }
            }
        }
    }

    // Could not find uncovered, force one from buffer
    for (auto i = 0; i < sudoku::eBufferSize; i += eConstraintTypes) {
        auto current = &constraintTable.buffer->at(i);
        if (sudoku::isLinkValues(current, row, col, num - '1')) {
            wrong_inputs.push_back(current);
            return;
        }
    }
}


void sudoku::SudokuPuzzle::recheckMistakes(sudoku::DancingLink *link) {
    if (sudoku::isUncovered(link)) {
        // Has removed
        /* if pencil mark is now uncovered it is valid, add to pencil marks*/
        for (auto i = wrong_marks.begin(), end = wrong_marks.end(); i < end; ) {
            if (sudoku::isUncovered(*i)) {
                pencilMarks.push_back(*i);
                i = wrong_marks.erase(i);
                end = wrong_marks.end();
            }
            else {
                i++;
            }
        }
    }
    else {
        // Has inserted
        /* manually remove pencil marks that are wrong and visible to the mark */
        for (auto i = wrong_marks.begin(), end = wrong_marks.end(); i < end; ) {
            if (canSee(link, *i)) {
                pencilMarks.push_back(*i);
                i = wrong_marks.erase(i);
                end = wrong_marks.end();
            }
            else {
                i++;
            }
        }
    }
    for (auto i = wrong_inputs.begin(), end = wrong_inputs.end(); i < end; ){
        if (sudoku::isUncovered(*i)) {
            // Cover instead of just putting in mistakes
            (*i)->colHeader->cover();
            sudoku::cover_link(*i);
            // No longer wrong so add to current
            constraintTable.current.push_back(*i);
            moves.push_back({Move::eCoverFull, *i});

            i = wrong_inputs.erase(i);
            end = wrong_inputs.end();
        }
        else {
            i++;
        }
    }
}

void sudoku::SudokuPuzzle::autoPencil() {
    pencilMarks.clear();
    wrong_marks.clear();
    for (auto col = constraintTable.root->right; col != constraintTable.root.get(); col = col->right) {
        for (auto row = col->down; row != col; row = row->down) {
            // Don't add to pencilMarks multiple times
            auto found = containsLinkEqual(row, pencilMarks.begin(), pencilMarks.end());
            if (found == pencilMarks.end()) {
                pencilMarks.push_back(row);
            }
        }
    }
}

void sudoku::SudokuPuzzle::getNextMove() {
    nextMove = sudoku::logic::getNextMove(*this, false);
}

void sudoku::removeFromPuzzle(sudoku::SudokuPuzzle *puzzle, int row, int col) {
    int cur_row, cur_col;

    // Checking if exists in wrong_inputs
    auto found = sudoku::containsLinkEquivalent(row, col, puzzle->wrong_inputs.begin(), puzzle->wrong_inputs.end());
    if (found != puzzle->wrong_inputs.end()) {
        puzzle->wrong_inputs.erase(found);
        return;
    }

    // Checking if exists in current
    found = sudoku::containsLinkEquivalent(row, col, puzzle->constraintTable.current.begin() + puzzle->current_start_index, puzzle->constraintTable.current.end());
    if (found == puzzle->constraintTable.current.end()) {
        // Does not exist
        return;
    }
    uncoverInVector(puzzle->moves, *found);
    puzzle->constraintTable.current.erase(found);
    puzzle->recheckMistakes(*found);
}

bool sudoku::canSee(sudoku::DancingLink *link_l, sudoku::DancingLink *link_r) {
    int row[2] = {sudoku::getRowFromLink(link_l), sudoku::getRowFromLink(link_r)};
    int col[2] = {sudoku::getColFromLink(link_l), sudoku::getColFromLink(link_r)};
    int num[2] = {sudoku::getNumFromLink(link_l), sudoku::getNumFromLink(link_r)};

    int constraints[2][sudoku::eConstraintTypes];
    for (auto i = 0; i < 2; i++) {
        sudoku::calculateConstraintColumns(constraints[i], row[i], col[i], num[i]);
    }

    for (auto i = 0; i < sudoku::eConstraintTypes; i++) {
        if (constraints[0][i] == constraints[1][i]) {
            return true;
        }
    }
    return false;
}

static void uncoverInVector(std::vector<sudoku::Move> &vector, sudoku::DancingLink *link) {
    /*
     * We need to uncover everything behind link and store the links
     * then we need to uncover link
     * then cover all the links that we have stored
     * when covering and uncovering remove and append to current
     */

    std::vector<sudoku::Move> uncovered;

    while(true) {
        auto current = vector.back();
        vector.pop_back();
        if (current.type == sudoku::Move::moveType::eCoverFull) {
            sudoku::uncover_link(current.link);
            current.link->colHeader->uncover();
            if (current.link == link) {
                // Dont add to uncovered
                break;
            }
        }
        else {
            sudoku::uncover_row(current.link);
            if (current.link == link) {
                // Don't add to covered
                break;
            }
        }
        uncovered.insert(uncovered.begin(), current);
    }

    for (auto l : uncovered) {
        if (l.type == sudoku::Move::moveType::eCoverFull) {
            l.link->colHeader->cover();
            cover_link(l.link);
        }
        else {
            sudoku::cover_row(l.link);
        }
        vector.push_back(l);
    }

}
