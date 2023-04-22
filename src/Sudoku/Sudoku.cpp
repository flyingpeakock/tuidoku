#include "Sudoku.h"

static bool canSee(Sudoku::DancingLink *link_l, Sudoku::DancingLink *link_r);

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

    found = Sudoku::containsLinkEqual(row, col, num - '1', wrong_marks.begin(), wrong_marks.end());
    if (found != wrong_marks.end()) {
        wrong_marks.erase(found);
        return;
    }

    found = Sudoku::containsLinkEqual(row, col, num - '1', pencilMarks.begin(), pencilMarks.end());
    if ((found != pencilMarks.end()) && (!Sudoku::isUncovered(*found))) {
        wrong_marks.push_back(*found);
        return;
    }

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
        wrong_inputs.erase(found);
        recheckMistakes(*found);
        if ((num - '1') == Sudoku::getNumFromLink(*found)) {
            // Removed if number already there
            return;
        }
    }
    else {
        // checking in current
        found = Sudoku::containsLinkEquivalent(row, col, constraintTable->current.begin() + current_start_index, constraintTable->current.end());
        if (found != constraintTable->current.end()) {
            Sudoku::uncoverInVector(constraintTable->current, *found);
            recheckMistakes(*found);
            if ((num - '1') == Sudoku::getNumFromLink(*found)) {
                // Removed if number is already there
                return;
            }
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
                    recheckMistakes(link);
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


void Sudoku::SudokuPuzzle::recheckMistakes(Sudoku::DancingLink *link) {
    if (Sudoku::isUncovered(link)) {
        // Has removed
        /* if pencil mark is now uncovered it is valid, add to pencil marks*/
        for (auto i = wrong_marks.begin(), end = wrong_marks.end(); i < end; ) {
            if (Sudoku::isUncovered(*i)) {
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
        if (Sudoku::isUncovered(*i)) {
            // Cover instead of just putting in mistakes
            (*i)->colHeader->cover();
            Sudoku::cover_link(*i);
            // No longer wrong so add to current
            constraintTable->current.push_back(*i);

            i = wrong_inputs.erase(i);
            end = wrong_inputs.end();
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
    puzzle->recheckMistakes(*found);
}

static bool canSee(Sudoku::DancingLink *link_l, Sudoku::DancingLink *link_r) {
    int row[2] = {Sudoku::getRowFromLink(link_l), Sudoku::getRowFromLink(link_r)};
    int col[2] = {Sudoku::getColFromLink(link_l), Sudoku::getColFromLink(link_r)};
    int num[2] = {Sudoku::getNumFromLink(link_l), Sudoku::getNumFromLink(link_r)};

    int constraints[2][4];
    for (auto i = 0; i < 2; i++) {
        Sudoku::calculateConstraintColumns(constraints[i], row[i], col[i], num[i]);
    }

    for (auto i = 0; i < 4; i++) {
        if (constraints[0][i] == constraints[1][i]) {
            return true;
        }
    }
    return false;
}
