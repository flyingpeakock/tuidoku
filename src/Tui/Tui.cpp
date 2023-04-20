#include "Tui.h"
#include <chrono>
#include <thread>

using namespace ftxui;

/**
 * @brief Draw the puzzle table, a puzzle with no filled in numbers
 * 
 * @param c Canvas to draw on, where the dimentions are at least width = 73 * 2, height = 37 * 4
 */
static void drawPuzzleTable(Canvas &c);

/**
 * @brief Draw a DancingLink * on the board
 * 
 * @param c canvas to draw on
 * @param link which link to draw
 * @param style which style to use
 */
static void drawFilledCell(Canvas &c, Sudoku::DancingLink *link, const Canvas::Stylizer &style);

/**
 * @brief Removed a link from the SudokuPuzzle type
 *
 * @param puzzle puzzle to remove from
 * @param row 0 - 8
 * @param col 0 - 8
 */
static void removeFromPuzzle(Tui::SudokuPuzzle *puzzle, int row, int col);

/**
 * @brief Draws a DancingLink * as a pencil mark on the board
 * 
 * @param c canvas to draw on
 * @param link which link to draw
 * @param style which style to use
 */
static void drawPencil(Canvas &c, Sudoku::DancingLink *link, const Canvas::Stylizer &style);

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

Tui::Board::Board(Sudoku::DancingLinkTable *table) :
    screen(ScreenInteractive::FitComponent()),
    row(0), col(0),
    puzzle(table),
    c(146, 148),
    state(eInsert),
    autoPencil(false),
    selected(0){

    cursor.shape = Screen::Cursor::Shape::Block;

    renderer = Renderer([&] {
        const Canvas::Stylizer style_clues = [&](Pixel &pixel){
            pixel.underlined = true;
            pixel.bold = true;

            if (pixel.character[0] == selected) {
                pixel.background_color = Color::Cyan;
                pixel.foreground_color = Color::White;
            }
        };

        const Canvas::Stylizer style_filled = [&](Pixel &pixel){
            pixel.bold = true;

            if (pixel.character[0] == selected) {
                pixel.background_color = Color::BlueLight;
                pixel.foreground_color = Color::White;
            }
        };

        const Canvas::Stylizer style_logical_error = [&](Pixel &pixel) {
            pixel.bold = true;
            pixel.blink = true;
            pixel.background_color = Color::Red1;
            pixel.foreground_color = Color::White;
        };

        const Canvas::Stylizer style_pencil = [&](Pixel &pixel) {
            pixel.dim = true;
            pixel.bold = false;

            if (pixel.character[0] == selected) {
                pixel.background_color = Color::BlueLight;
                pixel.foreground_color = Color::Black;
            }
        };

        const Canvas::Stylizer style_pencil_error = [&](Pixel &pixel) {
            pixel.dim = true;
            pixel.bold = false;
            pixel.background_color = Color::Red1;
            pixel.foreground_color = Color::White;
        };

        drawPuzzleTable(c);
        for (auto i = 0; i < puzzle.constraintTable->current.size(); i++) {
            Canvas::Stylizer style;
            if (i < puzzle.current_start_index) {
                style = style_clues;
            }
            else {
                style = style_filled;
            }
            drawFilledCell(c, puzzle.constraintTable->current[i], style);
        }

        for (const auto &err : puzzle.wrong_inputs) {
            drawFilledCell(c, err, style_logical_error);
        }

        if (autoPencil) {
            for (auto header = puzzle.constraintTable->root.right; header != &puzzle.constraintTable->root; header = header->right) {
                for (auto link = header->down; link != header; link = link->down) {
                    drawPencil(c, link, style_pencil);
                }
            }
        }
        else {
            for (auto &link : puzzle.pencilMarks) {
                drawPencil(c, link, style_pencil);
            }
            for (auto &link : puzzle.wrong_marks) {
                drawPencil(c, link, style_pencil_error);
            }
        }

        if (state == eInsert) {
            cursor.shape = Screen::Cursor::Shape::Block;
        }
        else {
            cursor.shape = Screen::Cursor::Shape::Bar;
        }
        cursor.x = 4 + (col * 8);
        cursor.y = 2 + (row * 4);
        screen.SetCursor(cursor);

        return frame(canvas(std::move(c)));
    });

    parseEvent = CatchEvent([&](Event event) {
        if (event.is_character()) {
            return parseKeys(event);
        }
        else if (event.is_mouse()) {
            auto m = event.mouse();
            if (m.button == Mouse::Button::Left) {
                col = ((m.x - 1) / 8);
                row = ((m.y - 1) / 4);
                return true;
            }
        }
        return false;
    });
}

void Tui::Board::playLoop() {
    screen.Loop(renderer | parseEvent);
}

bool Tui::Board::parseKeys(Event event) {
    bool key_pressed = false;
    if ((event == Event::Character("h")) || (event == Event::ArrowLeft)) {
        col--;
        key_pressed = true;
    }
    else if ((event == Event::Character("j")) || (event == Event::ArrowDown)) {
        row++;
        key_pressed = true;
    }
    else if ((event == Event::Character("k")) || (event == Event::ArrowUp)) {
        row--;
        key_pressed = true;
    }
    else if ((event == Event::Character("l")) || (event == Event::ArrowRight)) {
        col++;
        key_pressed = true;
    }
    else if (event == Event::Character("q")) {
        screen.ExitLoopClosure()();
        key_pressed = true;
    }
    else if (event == Event::Character("p")) {
        state = ePencil;
        key_pressed = true;
    }
    else if (event == Event::Character("i")) {
        state = eInsert;
        key_pressed = true;
    }
    else if (event == Event::Character("P")) {
        autoPencil = !autoPencil;
        key_pressed = true;
    }

    if (!key_pressed) {
        char pressed = event.character()[0];
        if ((pressed >= '0') && (pressed <= '9')) {
            if (state == eInsert) {
                puzzle.insert(row, col, pressed);
            }
            else if (!autoPencil){
                puzzle.pencil(row, col, pressed);
            }
            else {
                puzzle.pencilAuto(row, col, pressed);
            }

            selected = pressed;
            return true;
        }
        else if ((state == eInsert) && (event == Event::Character(" "))) {
            removeFromPuzzle(&puzzle, row, col);
            return true;
        }
    }
    else {
        if (col == 9) {
            col = 0;
        }
        else if (col < 0) {
            col = 8;
        }

        if (row == 9) {
            row = 0;
        }
        else if (row < 0) {
            row = 8;
        }
    }
    return key_pressed;
}

Tui::SudokuPuzzle::SudokuPuzzle(Sudoku::DancingLinkTable *table) :
    constraintTable(table),
    current_start_index(table->current.size()) {
}

void Tui::SudokuPuzzle::pencil(int row, int col, char num) {
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

void Tui::SudokuPuzzle::pencilAuto(int row, int col, char num) {
    /*
     * if in removed_marks uncover it
     * else cover it and add to removed_marks
     */

    auto found = Sudoku::containsLinkEqual(row, col, num - '1', removed_marks.begin(), removed_marks.end());
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

void Tui::SudokuPuzzle::insert(int row, int col, char num) {
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

static void drawPuzzleTable(Canvas &c) {
        const std::string toprow   = "╔═══════╤═══════╤═══════╦═══════╤═══════╤═══════╦═══════╤═══════╤═══════╗";
        const std::string rowtype1 = "║       │       │       ║       │       │       ║       │       │       ║";
        const std::string rowtype2 = "╟───────┼───────┼───────╫───────┼───────┼───────╫───────┼───────┼───────╢";
        const std::string rowtype3 = "╠═══════╪═══════╪═══════╬═══════╪═══════╪═══════╬═══════╪═══════╪═══════╣";
        const std::string botrow   = "╚═══════╧═══════╧═══════╩═══════╧═══════╧═══════╩═══════╧═══════╧═══════╝";

        auto style = [&] (Pixel &pixel) {
            pixel.bold = true;
        };

        int height = 0;
        c.DrawText(0, height, toprow, style);
        for (auto i = 0; i < 3; i++) {
            for (auto j = 0; j < 3; j++)
                c.DrawText(0, (++height) * 4, rowtype1, style);
            c.DrawText(0, (++height) * 4, rowtype2, style);
            for (auto j = 0; j < 3; j++)
                c.DrawText(0, (++height) * 4, rowtype1, style);
            c.DrawText(0, (++height) * 4, rowtype2, style);
            for (auto j = 0; j < 3; j++)
                c.DrawText(0, (++height) * 4, rowtype1, style);
            if (i != 2) {
                c.DrawText(0, (++height) * 4, rowtype3, style);
            }
        }
        c.DrawText(0, (++height) * 4, botrow, style);
}

static void drawFilledCell(Canvas &c, Sudoku::DancingLink *link, const Canvas::Stylizer &style){
    int row = Sudoku::getRowFromLink(link);
    int col = Sudoku::getColFromLink(link);
    int num = Sudoku::getNumFromLink(link);
    char num_char[2] = {(char)(num + '1'), '\0'};

    int x = 8 + (col * 16);
    int y = 8 + (row * 16);

    c.DrawText(x, y, num_char, style);
}

static void drawPencil(Canvas &c, Sudoku::DancingLink *link, const Canvas::Stylizer &style) {
    int row = Sudoku::getRowFromLink(link);
    int col = Sudoku::getColFromLink(link);
    int num = Sudoku::getNumFromLink(link);
    char num_char[2] = {(char)(num + '1'), '\0'};

    int x = 8 + (col * 16);
    int y = 8 + (row * 16);

    int x_offset = num % Sudoku::eBoxSize;
    int y_offset = num / Sudoku::eBoxSize;

    x = ((x - 4) + (4 * x_offset));
    y = ((y - 4) + (4 * y_offset));

    c.DrawText(x, y, num_char, style);
}

static void removeFromPuzzle(Tui::SudokuPuzzle *puzzle, int row, int col) {
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