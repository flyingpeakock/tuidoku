#include "Tui.h"
#include <chrono>
#include <thread>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/mouse.hpp>

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
 * @brief Draws a DancingLink * as a pencil mark on the board
 * 
 * @param c canvas to draw on
 * @param link which link to draw
 * @param style which style to use
 */
static void drawPencil(Canvas &c, Sudoku::DancingLink *link, const Canvas::Stylizer &style);

Tui::Board::Board(Sudoku::DancingLinkTable *table) :
    screen(ScreenInteractive::FitComponent()),
    row(0), col(0),
    puzzle(table),
    c(146, 148),
    state(eInsert),
    selected(0){

    cursor.shape = Screen::Cursor::Shape::Block;

    renderer = Renderer([&] {
        if (puzzle.constraintTable->root.right == &puzzle.constraintTable->root) {
            // Nothing to select if puzzle is done
            selected = 0;
        }

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
        for (auto &link : puzzle.pencilMarks) {
            // Don't draw covered
            if (Sudoku::isUncovered(link)) {
                // Don't draw if seen by any in wrong inputs
                bool is_hidden = false;
                for (auto &l : puzzle.wrong_inputs) {
                    if (Sudoku::canSee(l, link)) {
                        is_hidden = true;
                        break;
                    }
                }
                if (!is_hidden) {
                    drawPencil(c, link, style_pencil);
                }
            }
        }
        for (auto &link : puzzle.wrong_marks) {
            /*
            // Don't draw if in same cell as wrong input
            auto found = Sudoku::containsLinkEquivalent(Sudoku::getRowFromLink(link), Sudoku::getColFromLink(link), puzzle.wrong_inputs.begin(), puzzle.wrong_inputs.end());
            if (found == puzzle.wrong_inputs.end()) {
                drawPencil(c, link, style_pencil_error);
            }
            */
            // Don't draw if seen by any in wrong inputs
            bool is_hidden = false;
            for (auto &l : puzzle.wrong_inputs) {
                if (Sudoku::canSee(l, link)) {
                    is_hidden = true;
                    break;
                }
            }
            if (!is_hidden) {
                drawPencil(c, link, style_pencil_error);
            }
        }

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

        if (state == eInsert) {
            cursor.shape = Screen::Cursor::Shape::Block;
        }
        else {
            cursor.shape = Screen::Cursor::Shape::Bar;
        }
        cursor.x = 4 + (col * 8);
        cursor.y = 2 + (row * 4);
        screen.SetCursor(cursor);

        return canvas(std::move(c));
    });

    parseEvent = CatchEvent([&](Event event) {
        if (&puzzle.constraintTable->root == puzzle.constraintTable->root.right) {
            screen.ExitLoopClosure()();
            return true;
        }

        if (event.is_character()) {
            return parseKeys(event);
        }
        else if (event.is_mouse()) {
            return parseMouse(event);
        }
        return false;
    });
}

void Tui::Board::playLoop() {
    screen.Loop(renderer | parseEvent);
}

bool Tui::Board::parseMouse(Event event) {
    auto m = event.mouse();
    if (m.button == Mouse::Button::Left && m.motion == Mouse::Motion::Pressed) {
        col = ((m.x - 1) / 8);
        row = ((m.y - 1) / 4);

        if (col > 8) {
            col = 8;
        }
        else if (col < 0) {
            col = 0;
        }
        if (row > 8) {
            row = 8;
        }
        else if (row < 0) {
            row = 0;
        }

        auto found = Sudoku::containsLinkEquivalent(row, col, puzzle.constraintTable->current.begin(), puzzle.constraintTable->current.end());
        auto found_mistake = Sudoku::containsLinkEquivalent(row, col, puzzle.wrong_inputs.begin(), puzzle.wrong_inputs.end());
        if (found == puzzle.constraintTable->current.end() && found_mistake == puzzle.wrong_inputs.end()) {
            if (selected < '1' || selected > '9') {
                return true; // No number selected
            }
            // Pressed on empty cell, fill or pencil
            if (state == ePencil) {
                puzzle.pencil(row, col, selected);
            }
            else {
                // only insert if visible pencilmark
                found = Sudoku::containsLinkEqual(row, col, selected - '1', puzzle.pencilMarks.begin(), puzzle.pencilMarks.end());
                if (found == puzzle.pencilMarks.end()) { // not in pencil marks
                    found = Sudoku::containsLinkEqual(row, col, selected - '1', puzzle.wrong_marks.begin(), puzzle.wrong_marks.end());
                    if (found == puzzle.wrong_marks.end()) {
                        return true; // not a visible mark don't fill anything
                    }
                    else { // Found in wrong marks
                        bool isHidden = false;
                        for (auto &l : puzzle.wrong_inputs) {
                            if (Sudoku::canSee(l, *found)) {
                                isHidden = true;
                                break;
                            }
                        }
                        if (!isHidden) {
                            puzzle.insert(row, col, selected);
                        }
                    }
                }
                else { // Found in pencilmarks
                    // Contains regular mark, don't insert if covered
                    if (Sudoku::isUncovered(*found)) {
                        puzzle.insert(row, col, selected);
                    }
                }
            }
        }
        // Is filled
        else if (found != puzzle.constraintTable->current.end()) {
            selected = Sudoku::getNumFromLink(*found) + '1';
        }
        else if (found_mistake != puzzle.wrong_inputs.end()) {
            selected = Sudoku::getNumFromLink(*found) + '1';
        }
        return true;
    }
    return false;
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
        puzzle.autoPencil();
        key_pressed = true;
    }
    else if ((state == eInsert) &&
        ((event == Event::Character(" "))
        || (event == Event::Delete)
        || (event == Event::Backspace))) {
            Sudoku::removeFromPuzzle(&puzzle, row, col);
            key_pressed = true;
        }

    if (!key_pressed) {
        char pressed = event.character()[0];
        if ((pressed >= '0') && (pressed <= '9')) {
            if (state == eInsert) {
                puzzle.insert(row, col, pressed);
            }
            else {
                puzzle.pencil(row, col, pressed);
            }

            selected = pressed;
            key_pressed = true;
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
