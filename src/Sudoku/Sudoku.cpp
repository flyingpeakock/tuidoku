#include "Sudoku.h"

/**
 * @brief Check if mistakes are still mistakes after the board has been changed
 * 
 * @param mistakes vecetor containing mistakes
 * @param table constraint table
 */
static void recheckMistakes(std::vector<Sudoku::DancingLink *> &mistakes, Sudoku::DancingLinkTable *table);

Sudoku::SudokuPuzzle::SudokuPuzzle(Sudoku::DancingLinkTable *table) :
    constraintTable(table),
    current_start_index(table->current.size()) {
}

void Sudoku::SudokuPuzzle::pencil(int row, int col, char num) {
    if ((num < '1') || num > '9') {
        // invalid input
        return;
    }

    auto found = Sudoku::containsLinkEquivalent(row, col, constraintTable->current.begin(), constraintTable->current.end());
    if (found != constraintTable->current.end()) {
        return; // filled in square
    }
    found = Sudoku::containsLinkEquivalent(row, col, wrong_inputs.begin(), wrong_inputs.end());
    if (found != wrong_inputs.end()) {
        return; // filled in square
    }

    /*
     * if exists in removed_marks, uncover all rows until row backwards, then add row to removed_marks, then cover all rows from row forwards
     * if exists in pencilMarks, cover the row and add to removed_marks, mark is being removed
     * if exists in wrong_marks, simply remove from wrong_marks, mark is being removed
     * 
     * else search through constraint table
     *      if exists, add to pencilMarks
     *      if not exists, find in buffer and add to wrong_marks
     * 
     * if autopencil if exists in constraint table cover row and add to removed_marks
     * if autopencil and not exists in constraint table return
     * auto pencil should be handled in another function
     */

    found = Sudoku::containsLinkEqual(row, col, num - '1', removed_marks.begin(), removed_marks.end());
    if (found != removed_marks.end()) {
        // Putting back a mark that was removed
        pencilMarks.push_back(*found);
        std::vector<Sudoku::DancingLink *> needs_recover;
        while(true) {
            auto current = removed_marks.back();
            removed_marks.pop_back();
            Sudoku::uncover_row(current);
            if (current == *found) {
                break;
            }
            needs_recover.insert(needs_recover.begin(), current);
        }
        for (auto &p : needs_recover) {
            Sudoku::cover_row(p);
            removed_marks.push_back(p);
        }
        return;
    }

    found = Sudoku::containsLinkEqual(row, col, num - '1', pencilMarks.begin(), pencilMarks.end());
    if (found != pencilMarks.end()) {
        Sudoku::cover_row(*found);
        removed_marks.push_back(*found);
        pencilMarks.erase(found);
        return;
    }

    found = Sudoku::containsLinkEqual(row, col, num - '1', wrong_marks.begin(), wrong_marks.end());
    if (found != wrong_marks.end()) {
        wrong_marks.erase(found);
        return;
    }

    int constraints[4];
    Sudoku::calculateConstraintColumns(constraints, row, col, num - '1');
    for (auto loop = 0; loop < 2; loop++) {
        for (const auto &i : constraints) {
            auto colHeader = &constraintTable->colHeaders[i];
            if ((Sudoku::isUncovered(colHeader)) || (loop > 0)) {
                for (auto r = colHeader->down; r != colHeader; r = r->down) {
                    if (Sudoku::isLinkValues(r, row, col, num - '1')) {
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

void Sudoku::SudokuPuzzle::pencilAuto(int row, int col, char num) {
    auto found = Sudoku::containsLinkEquivalent(row, col, constraintTable->current.begin(), constraintTable->current.end());
    if (found != constraintTable->current.end()) {
        return; // can't pencil filled
    }
    found = Sudoku::containsLinkEquivalent(row, col, wrong_inputs.begin(), wrong_inputs.end());
    if (found != wrong_inputs.end()) {
        return; // can't pencil filled
    }
    /*
     * if in removed_marks uncover it
     * else cover it and add to removed_marks
     */

    found = Sudoku::containsLinkEqual(row, col, num - '1', removed_marks.begin(), removed_marks.end());
    if (found != removed_marks.end()) {
        std::vector<Sudoku::DancingLink *> needs_recover;
        while (true) {
            auto current = removed_marks.back();
            removed_marks.pop_back();
            Sudoku::uncover_row(current);

            if (current == *found) {
                break;
            }
            needs_recover.insert(needs_recover.begin(), current);
        }

        for (auto link : needs_recover) {
            Sudoku::cover_row(link);
        }
        return;
    }

    int constraints[4];
    Sudoku::calculateConstraintColumns(constraints, row, col, num - '1');

    for (const auto &i : constraints) {
        auto colHeader = &constraintTable->colHeaders[i];
        if (!Sudoku::isUncovered(colHeader)) continue;
        for (auto r = colHeader->down; r != colHeader; r = r->down) {
            if (Sudoku::isLinkValues(r, row, col, num - '1')) {
                Sudoku::cover_row(r);
                removed_marks.push_back(r);
                return;
            }
        }
    }
}

void Sudoku::SudokuPuzzle::insert(int row, int col, char num) {
    // Checking if position is a clue
    auto found = Sudoku::containsLinkEquivalent(row, col, constraintTable->current.begin(), constraintTable->current.begin() + current_start_index);
    if (found != constraintTable->current.begin() + current_start_index) {
        return;
    }

    if (num == '0') {
        return removeFromPuzzle(this, row, col);
    }
    if ((num < '1') || num > '9') {
        // invalid input
        return;
    }

    // Checking if position already filled
    // in wrong input
    found = Sudoku::containsLinkEquivalent(row, col, wrong_inputs.begin(), wrong_inputs.end());
    if (found != wrong_inputs.end()) {
        if ((num - '1') == Sudoku::getNumFromLink(*found)) {
            // Already exists
            return;
        }
        wrong_inputs.erase(found);
    }
    else {
        // checking in current
        found = Sudoku::containsLinkEquivalent(row, col, constraintTable->current.begin() + current_start_index, constraintTable->current.end());
        if (found != constraintTable->current.end()) {
            if ((num - '1') == Sudoku::getNumFromLink(*found)) {
                // Already exists
                return;
            }
            Sudoku::uncoverInVector(constraintTable->current, *found);
            recheckMistakes(wrong_inputs, constraintTable);
        }
    }

    // The cell at row col is now empty
    // Check if it is missing from constraint table because of removed pencil
    found = Sudoku::containsLinkEqual(row, col, num - '1', removed_marks.begin(), removed_marks.end());
    if (found != removed_marks.end()) {
        auto f = *found;
        std::vector<DancingLink *> re_cover;
        while (true) {
            auto current = removed_marks.back();
            removed_marks.pop_back();
            uncover_row(current);
            if (current = f) break;
            re_cover.insert(re_cover.begin(), current);
        }
        for (auto &current : re_cover) {
            cover_row(current);
        }
    }

    // Add chosen link to current and cover
    int constraints[4];
    Sudoku::calculateConstraintColumns(constraints, row, col, num - '1');
    for (auto i : constraints) {
        if (Sudoku::isUncovered(&constraintTable->colHeaders[i])) {
            for (auto link = constraintTable->colHeaders[i].down; link != &constraintTable->colHeaders[i]; link = link->down) {
                if (Sudoku::isLinkValues(link, row, col, num - '1')) {
                    link->colHeader->cover();
                    Sudoku::cover_link(link);
                    constraintTable->current.push_back(link);

                    // we need to remove links in pencilMarks that are now covered
                    for (auto j = pencilMarks.begin(), end = pencilMarks.end(); j < end;) {
                        if (Sudoku::isUncovered(*j)) {
                            j++;
                        }
                        else {
                            j = pencilMarks.erase(j);
                            end = pencilMarks.end();
                        }
                    }
                    return;
                }
            }
        }
    }

    // Could not find uncovered, force one from buffer
    for (auto i = 0; i < Sudoku::eBufferSize; i += 4) {
        auto current = &constraintTable->buffer[i];
        if (Sudoku::isLinkValues(current, row, col, num - '1')) {
            wrong_inputs.push_back(current);
            return;
        }
    }
}


static void recheckMistakes(std::vector<Sudoku::DancingLink *> &mistakes, Sudoku::DancingLinkTable *table) {
    for (auto i = mistakes.begin(), end = mistakes.end(); i < end; ){
        if (Sudoku::isUncovered(*i)) {
            // Cover instead of just putting in mistakes
            (*i)->colHeader->cover();
            Sudoku::cover_link(*i);
            table->current.push_back(*i);

            i = mistakes.erase(i);
            end = mistakes.end();
        }
        else {
            i++;
        }
    }
}

void Sudoku::removeFromPuzzle(Sudoku::SudokuPuzzle *puzzle, int row, int col) {
    int cur_row, cur_col;

    // Checking if exists in wrong_inputs
    auto found = Sudoku::containsLinkEquivalent(row, col, puzzle->wrong_inputs.begin(), puzzle->wrong_inputs.end());
    if (found != puzzle->wrong_inputs.end()) {
        puzzle->wrong_inputs.erase(found);
        return;
    }

    // Checking if exists in current
    found = Sudoku::containsLinkEquivalent(row, col, puzzle->constraintTable->current.begin() + puzzle->current_start_index, puzzle->constraintTable->current.end());
    if (found == puzzle->constraintTable->current.end()) {
        // Does not exist
        return;
    }

    Sudoku::uncoverInVector(puzzle->constraintTable->current, *found);
    recheckMistakes(puzzle->wrong_inputs, puzzle->constraintTable);
}
