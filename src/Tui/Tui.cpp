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

/**
 * @brief Draws all the visible pencilMarks
 * 
 * @param c Canvas to draw on
 * @param selected character that is currently selected '1' - '9'
 * @param pencils vector containing links of pencilmarks
 * @param pencilError vector containing links of pencilmarks that are illogical
 * @param wrong_inputs vector containing links of inputs that are illogical
 */
static void drawAllPencils(Canvas &c,
                           const char selected,
                           const std::vector<Sudoku::DancingLink *> &pencils,
                           const std::vector<Sudoku::DancingLink *> &pencilError,
                           const std::vector<Sudoku::DancingLink *> &wrong_inputs);

/**
 * @brief Draws all the visiable inputs
 * 
 * @param c Canvas to draw on
 * @param selected Character that is currenly is selected '1' - '9'
 * @param inputs vector containing link of inputs
 * @param errors vector containing links of illogical inputs
 * @param current_start_index index where inputs turn from clues to guesses
 */
static void drawAllFilled(Canvas &c, 
                          const char selected,
                          const std::vector<Sudoku::DancingLink *> &inputs,
                          const std::vector<Sudoku::DancingLink *> &errors,
                          const int current_start_index);

/**
 * @brief Set the Cursor shape and position
 * 
 * @param s screen to draw cursor on
 * @param row row in puzzle
 * @param col column in puzzle
 * @param isPencil is in pencil mode
 */
static void setCursor(Screen &s, const int row, const int col, bool isPencil);

/**
 * @brief checks if the puzzle is completed
 * 
 * @param root root of the contraint table
 * @return true if not done
 * @return false if done
 */
static inline bool isPlaying(Sudoku::DancingLink *root) {
    return root->right != root;
}

/**
 * @brief Construct a new Tui:: Board:: Board object
 * 
 * @param table constraint table that represents the puzzle
 */
Tui::Board::Board(Sudoku::DancingLinkTable *table) :
    screen(ScreenInteractive::FitComponent()),
    row(0), col(0),
    puzzle(table),
    c(146, 148),
    state(eInsert),
    selected(0),

    renderer(Renderer([&] {
        if (!isPlaying(&puzzle.constraintTable->root)) {
            // Nothing to select if puzzle is done
            selected = 0;
        }

        drawPuzzleTable(c);
        drawAllPencils(c, selected, puzzle.pencilMarks, puzzle.wrong_marks, puzzle.wrong_inputs);
        drawAllFilled(c, selected, puzzle.constraintTable->current, puzzle.wrong_inputs, puzzle.current_start_index);
        setCursor(screen, row, col, state == ePencil);

        return canvas(std::move(c));
    })),

    parseEvent(CatchEvent([&](Event event) {
        if (!isPlaying(&puzzle.constraintTable->root)) {
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
    })){}

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


static void drawAllPencils(Canvas &c,
                           const char selected,
                           const std::vector<Sudoku::DancingLink *> &pencils,
                           const std::vector<Sudoku::DancingLink *> &pencilError,
                           const std::vector<Sudoku::DancingLink *> &wrong_inputs) {
                        
    
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

    for (auto &link : pencils) {
        // Don't draw covered
        if (Sudoku::isUncovered(link)) {
            // Don't draw if seen by any in wrong inputs
            bool is_hidden = false;
            for (auto &l : wrong_inputs) {
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
    for (auto &link : pencilError) {
        // Don't draw if seen by any in wrong inputs
        bool is_hidden = false;
        for (auto &l : wrong_inputs) {
            if (Sudoku::canSee(l, link)) {
                is_hidden = true;
                break;
            }
        }
        if (!is_hidden) {
            drawPencil(c, link, style_pencil_error);
        }
    }
}

static void drawAllFilled(Canvas &c, 
                          const char selected,
                          const std::vector<Sudoku::DancingLink *> &inputs,
                          const std::vector<Sudoku::DancingLink *> &errors,
                          const int current_start_index){

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

    for (auto i = 0; i < inputs.size(); i++) {
        Canvas::Stylizer style;
        if (i < current_start_index) {
            style = style_clues;
        }
        else {
            style = style_filled;
        }
        drawFilledCell(c, inputs[i], style);
    }

    for (const auto &err : errors) {
        drawFilledCell(c, err, style_logical_error);
    }
}

static void setCursor(Screen &s, const int row, const int col, bool isPencil) {
    static Screen::Cursor cursor;
    if (!isPencil) {
        cursor.shape = Screen::Cursor::Shape::Block;
    }
    else {
        cursor.shape = Screen::Cursor::Shape::Bar;
    }
    cursor.x = 4 + (col * 8);
    cursor.y = 2 + (row * 4);
    s.SetCursor(cursor);
}
