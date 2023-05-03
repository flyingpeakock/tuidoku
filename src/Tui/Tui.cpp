#include "Tui.h"
#include <chrono>
#include <thread>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/mouse.hpp>
#include <ftxui/component/loop.hpp>

using namespace ftxui;

/**
 * @brief Draw the puzzle table, a puzzle with no filled in numbers
 * 
 * @param c Canvas to draw on, where the dimentions are at least width = 73 * 2, height = 37 * 4
 */
static void drawPuzzleTable(Tui::PuzzleCanvas &c);

/**
 * @brief Draw a DancingLink * on the board
 * 
 * @param c canvas to draw on
 * @param link which link to draw
 * @param style which style to use
 */
static void drawFilledCell(Tui::PuzzleCanvas &c, Sudoku::DancingLink *link, const Canvas::Stylizer &style);

/**
 * @brief Draws a DancingLink * as a pencil mark on the board
 * 
 * @param c canvas to draw on
 * @param link which link to draw
 * @param style which style to use
 */
static void drawPencil(Tui::PuzzleCanvas &c, Sudoku::DancingLink *link, const Canvas::Stylizer &style);

/**
 * @brief Draws all the visible pencilMarks
 * 
 * @param c Canvas to draw on
 * @param selected character that is currently selected '1' - '9'
 * @param pencils vector containing links of pencilmarks
 * @param pencilError vector containing links of pencilmarks that are illogical
 * @param wrong_inputs vector containing links of inputs that are illogical
 */
static void drawAllPencils(Tui::PuzzleCanvas &c,
                           const char selected,
                           const Sudoku::DancingLinkContainer &pencils,
                           const Sudoku::DancingLinkContainer &pencilError,
                           const Sudoku::DancingLinkContainer &wrong_inputs);

/**
 * @brief Draws all the visiable inputs
 * 
 * @param c Canvas to draw on
 * @param selected Character that is currenly is selected '1' - '9'
 * @param inputs vector containing link of inputs
 * @param errors vector containing links of illogical inputs
 * @param current_start_index index where inputs turn from clues to guesses
 */
static void drawAllFilled(Tui::PuzzleCanvas &c, 
                          const char selected,
                          const Sudoku::DancingLinkContainer &inputs,
                          const Sudoku::DancingLinkContainer &errors,
                          const int current_start_index);

/**
 * @brief Print the next move on the board
 * 
 * @param c Canvas to draw on
 * @param move Sudoku::logic::Move object to draw
 */
static void drawNextMove(Tui::PuzzleCanvas &c, const Sudoku::logic::LogicalMove &move);

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
 * @brief Set the row and column on mouse clicks
 * 
 * @param row reference to the variable that gets set with the row
 * @param col  reference to the variable that gets set with the column
 * @param m_row y position clicked by mouse
 * @param m_col x position clicked by mouse
 */
static void setMouseRowCol(int &row, int &col, int m_row, int m_col);

Tui::PuzzleCanvas::PuzzleCanvas() :
    Canvas(146, 148),
    selected(0), row(0), col(0)
{
}

void Tui::PuzzleCanvas::DrawPuzzle(const Sudoku::SudokuPuzzle &puzzle) {
    drawPuzzleTable(*this);
    drawAllPencils(*this, selected, puzzle.pencilMarks, puzzle.wrong_marks, puzzle.wrong_inputs);
    drawAllFilled(*this, selected, puzzle.constraintTable.current, puzzle.wrong_inputs, puzzle.current_start_index);
    if (puzzle.nextMove.type != Sudoku::logic::eMoveNotFound) {
        drawNextMove(*this, puzzle.nextMove);
    }
}

bool Tui::PuzzleCanvas::parseMouse(Event event) {
    auto m = event.mouse();
    if (m.button == Mouse::Button::None) {
        return false;
    }
    if (m.motion != Mouse::Motion::Pressed) {
        return false;
    }
    setMouseRowCol(row, col, m.y, m.x);
    return true;
}

bool Tui::PuzzleCanvas::parseKeys(Event event) {
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
    if (key_pressed) {
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

ftxui::Element Tui::PuzzleCanvas::getCanvas() {
    return canvas(static_cast<ftxui::Canvas>(*this));
}

Tui::Tui::Tui() :
    screen(ScreenInteractive::FitComponent()),
    difficulty(Sudoku::eAny),
    state(eMenu),
    table(false),
    puzzle(table)
{
}

void Tui::Tui::runLoop() {
    int tab_drawn = 0;

    /* Add new difficulties here */
    std::vector<std::string> difficulties = {
        "Beginner",
        "Easy",
        "Medium",
        "Difficult",
        "Expert",
        "Pro",
        "Any",
        "Exit"
    };

    /* Gets set to the chosen index in menu */
    int choice = 0;

    /* Setting menu style */
    MenuOption option = MenuOption::Vertical();
    option.entries.transform = [&] (EntryState state) {
        state.label = (state.active) ? " ▶ " + state.label : "   " + state.label;
        Element e = hbox(text(state.label), filler());
        if (state.label.find("Exit") != std::string::npos) {
            if (state.active) {
                e = e | bold | bgcolor(Color::Red);
            }
            e = vbox(separator(), e);
        }
        else if (state.active) {
            e = e | bold | bgcolor(Color::Green);
        }
        return e;
    };

    /* Setting menu behaviour*/
    option.on_enter = [&]{
        parseMenuChoice(choice);
    };

    /* Creating menu title and border */
    auto difficulty_menu = Menu(&difficulties, &choice, &option);
    auto menu_renderer = Renderer(difficulty_menu, [&]{
        return vbox(text("Select difficulty") | bold, separator(), difficulty_menu->Render()) | borderDouble;
    });

    /* Board renderer */
    auto board_renderer = Renderer([&]{
        puzzleCanvas.DrawPuzzle(puzzle);
        return canvas(std::move(puzzleCanvas));
    });

    /* Parse input for everything */
    parseInput = CatchEvent([&](Event event){
        if (state == eMenu) {
            if (event == Event::Character('q')) {
                puzzleCanvas.selected = 0;
                puzzle.nextMove.type = Sudoku::logic::eMoveNotFound;
                state = eExit;
            }
            return false; // Use defualt menu keys
        }
        if (table.root->right == table.root.get()) {
            state = eExit;
            puzzleCanvas.selected = 0;
            puzzle.nextMove.type = Sudoku::logic::eMoveNotFound;
            return true;
        }
        return parseEvent(event, puzzle);
    });


    /* Tab to choose which renderer to render */
    auto container = Container::Tab({
        board_renderer, menu_renderer
    }, &tab_drawn) | parseInput;

    /* Main renderer */
    auto renderer = Renderer(container, [&] {
        if (state == eMenu) {
            tab_drawn = 1;
        }
        else {
            tab_drawn = 0;
            setCursor(screen, puzzleCanvas.row, puzzleCanvas.col, state == ePencil);
        }

        if (state == eExit) {
            screen.ExitLoopClosure()();
        }
        return container->Render();
    });

    screen.Loop(renderer);
}

void Tui::Tui::parseMenuChoice(int choice) {
    switch (choice) {
        case 0:
            difficulty = Sudoku::eBeginner;
            state = eInsert;
            break;
        case 1:
            difficulty = Sudoku::eEasy;
            state = eInsert;
            break;
        case 2:
            difficulty = Sudoku::eMedium;
            state = eInsert;
            break;
        case 3:
            difficulty = Sudoku::eHard;
            state = eInsert;
            break;
        case 4:
            difficulty = Sudoku::eExpert;
            state = eInsert;
            break;
        case 5:
            difficulty = Sudoku::ePro;
            state = eInsert;
            break;
        case 6:
            difficulty = Sudoku::eAny;
            state = eInsert;
            break;
        case 7:
            state = eExit;
            break;
    }
    if (state == eInsert) {
        table = Sudoku::generate(difficulty);
        puzzle = Sudoku::SudokuPuzzle(table);
    }
}

bool Tui::Tui::parseEvent(Event event, Sudoku::SudokuPuzzle &puzzle) {
    bool key_pressed = false;
    if (event.is_character()) {
        if (puzzleCanvas.parseKeys(event)) {
            key_pressed = true;
        }
        else if (event == Event::Character("q")) {
            puzzleCanvas.selected = 0;
            if (state != eMenu) {
                state = eMenu;
            }
            else {
                state = eExit;
            }
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
        else if (event == Event::Character("H")) {
            if (puzzle.nextMove.type == Sudoku::logic::eMoveNotFound) {
                puzzle.getNextMove();
                key_pressed = true;
            }
            else {
                puzzle.nextMove.type = Sudoku::logic::eMoveNotFound;
            }
        }
        else if ((state == eInsert) &&
            ((event == Event::Character(" "))
            || (event == Event::Delete)
            || (event == Event::Backspace))) {
                Sudoku::removeFromPuzzle(&puzzle, puzzleCanvas.row, puzzleCanvas.col);
                key_pressed = true;
            }
        if (!key_pressed) {
            char pressed = event.character()[0];
            if ((pressed >= '0') && (pressed <= '9')) {
                if (state == eInsert) {
                    puzzle.insert(puzzleCanvas.row, puzzleCanvas.col, pressed);
                }
                else {
                    puzzle.pencil(puzzleCanvas.row, puzzleCanvas.col, pressed);
                }

                puzzleCanvas.selected = pressed;
                key_pressed = true;
            }
        }
    }
    else if (event.is_mouse()) {
        key_pressed = puzzleCanvas.parseMouse(event);
        if (key_pressed && event.mouse().button == Mouse::Button::Left) {
            auto row = puzzleCanvas.row;
            auto col = puzzleCanvas.col;
            auto selected = puzzleCanvas.selected;

            auto found = Sudoku::containsLinkEquivalent(row, col, puzzle.constraintTable.current.begin(), puzzle.constraintTable.current.end());
            if (found != puzzle.constraintTable.current.end()) {
                selected = Sudoku::getNumFromLink(*found) + '1';
                return true; // This cell is filled
            }
            found = Sudoku::containsLinkEquivalent(row, col, puzzle.wrong_inputs.begin(), puzzle.wrong_inputs.end());
            if (found != puzzle.wrong_inputs.end()) {
                selected = Sudoku::getNumFromLink(*found) + '1';
                return true; // This cell is filled
            }

            // If made it here pressed on an empty cell
            if (selected < '1' || selected > '9') {
                return true; // No number selected
            }

            if (state == ePencil) {
                puzzle.pencil(row, col, selected);
                return true;
            }

            // only insert if visible pencilmark
            found = Sudoku::containsLinkEqual(row, col, selected - '1', puzzle.pencilMarks.begin(), puzzle.pencilMarks.end());
            if (found != puzzle.pencilMarks.end() && Sudoku::isUncovered(*found)) {
                puzzle.insert(row, col, selected);
                return true;
            }
            found = Sudoku::containsLinkEqual(row, col, selected - '1', puzzle.wrong_marks.begin(), puzzle.wrong_marks.end());
            if (found != puzzle.wrong_marks.end()) {
                for (auto &l : puzzle.wrong_inputs) {
                    if (Sudoku::canSee(l, *found)) {
                        return true; // Not a visible mark
                    }
                }
                puzzle.insert(row, col, selected);
                return true;
            }
        }
    }
    return key_pressed;
}

static void drawPuzzleTable(Tui::PuzzleCanvas &c) {
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

static void drawFilledCell(Tui::PuzzleCanvas &c, Sudoku::DancingLink *link, const Canvas::Stylizer &style){
    int row = Sudoku::getRowFromLink(link);
    int col = Sudoku::getColFromLink(link);
    int num = Sudoku::getNumFromLink(link);
    char num_char[2] = {(char)(num + '1'), '\0'};

    int x = 8 + (col * 16);
    int y = 8 + (row * 16);

    c.DrawText(x, y, num_char, style);
}

static void drawPencil(Tui::PuzzleCanvas &c, Sudoku::DancingLink *link, const Canvas::Stylizer &style) {
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


static void drawAllPencils(Tui::PuzzleCanvas &c,
                           const char selected,
                           const Sudoku::DancingLinkContainer &pencils,
                           const Sudoku::DancingLinkContainer &pencilError,
                           const Sudoku::DancingLinkContainer &wrong_inputs) {
                        
    
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

static void drawAllFilled(Tui::PuzzleCanvas &c, 
                          const char selected,
                          const Sudoku::DancingLinkContainer &inputs,
                          const Sudoku::DancingLinkContainer &errors,
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

static void setMouseRowCol(int &row, int &col, int m_row, int m_col) {
    col = ((m_col - 1) / 8);
    row = ((m_row - 1) / 4);

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
}

static void drawNextMove(Tui::PuzzleCanvas &c, const Sudoku::logic::LogicalMove &move) {
    const Canvas::Stylizer style_truth = [&](Pixel &pixel) {
        pixel.dim = true;
        pixel.bold = false;
        pixel.blink = true;
        pixel.background_color = Color::Green;
    };

    const Canvas::Stylizer style_false = [&](Pixel &pixel) {
        pixel.dim = true;
        pixel.bold = false;
        pixel.blink = true;
        pixel.background_color = Color::Red3;
    };

    const Canvas::Stylizer style_false_filled = [&](Pixel &pixel) {
        pixel.dim = false;
        pixel.bold = true;
        pixel.blink = true;
        pixel.background_color = Color::Red3;
    };

    for (auto link : move.falses) {
        if (!Sudoku::isUncovered(link)) continue;
        if (move.type == Sudoku::logic::eLogicErrorInsert) {
            drawFilledCell(c, link, style_false_filled);
        }
        else {
            drawPencil(c, link, style_false);
        }
    }

    for (auto link : move.truths) {
        drawPencil(c, link, style_truth);
    }
}
