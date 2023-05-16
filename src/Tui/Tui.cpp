#include "Tui.h"

#include <ftxui/dom/elements.hpp>                 // for operator|, text
#include <ftxui/component/event.hpp>              // for Event
#include <ftxui/component/mouse.hpp>              // for Mouse
#include <ftxui/component/loop.hpp>               // for Loop
#include <bits/chrono.h>                          // for operator""ms, chron...
#include <cxxabi.h>                               // for __forced_unwind
#include <ftxui/component/component_options.hpp>  // for EntryState, MenuOption
#include <ftxui/dom/canvas.hpp>                   // for Canvas
#include <ftxui/dom/flexbox_config.hpp>           // for FlexboxConfig
#include <ftxui/screen/box.hpp>                   // for ftxui
#include <ftxui/screen/color.hpp>                 // for Color
#include <ftxui/screen/screen.hpp>                // for Pixel, Screen
#include <ctime>                                  // for time
#include <thread>                                 // for sleep_for
#include <atomic>                                 // for atomic
#include <functional>                             // for function
#include <memory>                                 // for allocator, __shared...
#include <string>                                 // for string, char_traits
#include <system_error>                           // for system_error
#include <utility>                                // for move
#include <vector>                                 // for vector

#include "Sudoku/DancingLink.h"                   // for getNumFromLink, isU...
#include "Sudoku/Sudoku.h"                        // for canSee, generate
#include "ftxui/dom/elements.hpp"                 // for hbox, vbox, dbox

using namespace ftxui;

/**
 * @brief Draws a sudoku puzzzle on a canvas
 * 
 * @param puzzleCanvas canvas to draw on
 * @param puzzle constraint table
 * @param selected value to highlight
 */
void DrawPuzzle(Canvas &puzzleCanvas, const Sudoku::SudokuPuzzle &puzzle, char selected);


/**
 * @brief Set the row and column on mouse clicks
 * 
 * @param row reference to the variable that gets set with the row
 * @param col  reference to the variable that gets set with the column
 * @param m_row y position clicked by mouse
 * @param m_col x position clicked by mouse
 */
static void setMouseRowCol(int &row, int &col, int m_row, int m_col);

/**
 * @brief Create a frame with a double border the same size as a sudoku puzzle
 * 
 * @param inner element that should be in the frame
 * @param focus where the focus is on the frame
 * @return Element frame
 */
static Element sudokuFrame(Element inner, float focus);

static Element helpElement(float focus_y);

bool parseMouse(Event event, int &row, int &col) {
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

Tui::Tui::Tui() :
    screen(ScreenInteractive::FitComponent()),
    difficulty(Sudoku::eAny),
    state(eMenu),
    table(false),
    puzzle(table),
    generating_puzzle(false),
    row(0),
    col(0),
    selected(0),
    focus_y(0.0f),
    parseInput(CatchEvent([&](Event event){
        return parseEvent(event, puzzle);
    })),
    board_renderer(Renderer([&]{
        Canvas puzzleCanvas(146, 148);
        DrawPuzzle(puzzleCanvas, puzzle, selected);
        return canvas(std::move(puzzleCanvas));
    })),
    help_renderer(Renderer([&]{
        return helpElement(focus_y);
    }))
{
}

void Tui::Tui::runLoop() {
    int tab_drawn = 0;
    std::string table_text = "Choose Puzzle";

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
    static int choice = 0;

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
            e = e | bold | bgcolor(Color::DarkOrange);
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
        return dbox(board_renderer->Render(),
                    gridbox({
                        {filler() | size(HEIGHT, EQUAL, 6)},
                        {filler() | size(WIDTH, EQUAL, 12), vbox(text(table_text) | bold, separator(), difficulty_menu->Render()) | borderDouble | clear_under},
                    })
                   );
    });

    /* Tab to choose which renderer to render */
    auto container = Container::Tab({
        board_renderer, menu_renderer, help_renderer
    }, &tab_drawn) | parseInput;

    /* Main renderer */
    auto renderer = Renderer(container, [&] {
        if (state == eMenu) {
            if (generating_puzzle) {
                table_text = " Generating Puzzle";
                auto now = std::time(nullptr);
                for (auto i = 0; i < (now % 4); i++) {
                    table_text = table_text + '.';
                }
            }
            else {
                table_text = " Choose Puzzle";
            }

            while (table_text.size() < std::string(" Generating Puzzle...").size()) {
                table_text = table_text + ' ';
            }
            tab_drawn = 1;
        }
        else if (state == eHelp) {
            tab_drawn = 2;
        }
        else {
            tab_drawn = 0;
        }

        if (state == eExit) {
            screen.ExitLoopClosure()();
        }
        setCursor();

        return container->Render();
    });

    Loop loop(&screen, renderer);
    while (!loop.HasQuitted()) {
        if (generating_puzzle) {
            loop.RunOnce();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            screen.PostEvent(Event::Home); // Random unused key to refresh screen
        }
        else {
            loop.RunOnceBlocking();
        }
    }
    Sudoku::kill_threads = true;
}

void Tui::Tui::parseMenuChoice(int choice) {
    if (choice == 7) {
        state = eExit;
        return;
    }
    if (generating_puzzle) {
        return;
    }
    switch (choice) {
        case 0:
            difficulty = Sudoku::eBeginner;
            break;
        case 1:
            difficulty = Sudoku::eEasy;
            break;
        case 2:
            difficulty = Sudoku::eMedium;
            break;
        case 3:
            difficulty = Sudoku::eHard;
            break;
        case 4:
            difficulty = Sudoku::eExpert;
            break;
        case 5:
            difficulty = Sudoku::ePro;
            break;
        case 6:
            difficulty = Sudoku::eAny;
            break;
    }
    if (choice <= 6) {
        table_promise = std::async(std::launch::async, [&]{
            return Sudoku::generate(difficulty);
        });
        generating_puzzle = true;
    }
}

bool Tui::Tui::parseEvent(Event event, Sudoku::SudokuPuzzle &puzzle) {
    bool key_pressed = false;
    static stateEnum previousState = eInsert;
    if (event == Event::Character('?')) {
        if (state == eHelp) {
            state = previousState;
        }
        else {
            previousState = state;
            state = eHelp;
        }
        return true;
    }

    if (event == Event::Character("q")) {
        selected = 0;
        if (state != eMenu) {
            state = eMenu;
        }
        else {
            state = eExit;
        }
        return true;
    }

    switch (state) {
        case eMenu:
            if (event == Event::Character('q')) {
                selected = 0;
                puzzle.nextMove.type = Sudoku::logic::eMoveNotFound;
                state = eExit;
                key_pressed = true;
            }
            else if (generating_puzzle) {
                using namespace std::chrono_literals;
                auto status = table_promise.wait_for(0ms);
                if (status == std::future_status::ready) {
                    table = table_promise.get();
                    puzzle = Sudoku::SudokuPuzzle(table);
                    state = eInsert;
                    generating_puzzle = false;
                }
                key_pressed = true;
            }
            break;
        
        case eHelp:
            if (event.is_character()) {
                if (event == Event::Character('k')) {
                    focus_y -= 0.1f;
                    if (focus_y > 0.7f) { focus_y = 0.6f; }
                }
                else if (event == Event::Character('j')) {
                    focus_y += 0.1f;
                    if (focus_y < 0.3f) {focus_y = 0.4f; }
                }
                else {
                    focus_y = 0.0f;
                    state = previousState;
                }
                key_pressed = true;
            }
            else if (event.is_mouse()) {
                auto m = event.mouse();
                if (m.button == Mouse::Button::WheelUp) {
                    focus_y -= 0.1f;
                    if (focus_y > 0.7f) { focus_y = 0.6f; }
                    key_pressed = true;
                }
                else if (m.button == Mouse::Button::WheelDown) {
                    focus_y += 0.1f;
                    if (focus_y < 0.3f) {focus_y = 0.4f; }
                    key_pressed = true;
                }
                else if (m.button != Mouse::Button::None && m.motion == Mouse::Motion::Pressed) {
                    focus_y = 0.0f;
                    state = previousState;
                    key_pressed = true;
                }
            }
            break;

        case eInsert: // Fallthrough
            if (event.is_character()) {
                if ((event == Event::Character(" "))
                    || (event == Event::Delete)
                    || (event == Event::Backspace)) {

                        Sudoku::removeFromPuzzle(&puzzle, row, col);
                        key_pressed = true;
                        break;
                }
            }

        case ePencil:
            // Games is done
            if (table.root->right == table.root.get()) {
                state = eMenu;
                selected = 0;
                puzzle.nextMove.type = Sudoku::logic::eMoveNotFound;
                key_pressed = true;
                break;
            }

            if (event.is_character()) {
                if (event == Event::Character("h")) {
                    col--;
                    key_pressed = true;
                }
                else if (event == Event::Character("j")) {
                    row++;
                    key_pressed = true;
                }
                else if (event == Event::Character("k")) {
                    row--;
                    key_pressed = true;
                }
                else if (event == Event::Character("l")) {
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
                    }
                    else {
                        puzzle.nextMove.type = Sudoku::logic::eMoveNotFound;
                    }
                    key_pressed = true;
                }

                if (!key_pressed) {
                    char pressed = event.character()[0];
                    if ((pressed >= '0') && (pressed <= '9')) {
                        if (state == eInsert) {
                            puzzle.insert(row, col, pressed);
                        }
                        else if (state == ePencil) {
                            puzzle.pencil(row, col, pressed);
                        }
                        selected = pressed;
                        key_pressed = true;
                    }
                }
            }
            else if (event.is_mouse()) {
                key_pressed = parseMouse(event, row, col);
                if (key_pressed && event.mouse().button == Mouse::Button::Left) {
                    // Insert or pencil as well
                    auto found = Sudoku::containsLinkEquivalent(row, col, puzzle.constraintTable.current.begin(), puzzle.constraintTable.current.end());
                    if (found != puzzle.constraintTable.current.end()) {
                        selected = Sudoku::getNumFromLink(*found) + '1';
                        break;
                    }
                    found = Sudoku::containsLinkEquivalent(row, col, puzzle.wrong_inputs.begin(), puzzle.wrong_inputs.end());
                    if (found != puzzle.wrong_inputs.end()) {
                        selected = Sudoku::getNumFromLink(*found) + '1';
                        break;
                    }

                    if (selected < '1' || selected > '9') {
                        break;
                    }

                    if (state == ePencil) {
                        puzzle.pencil(row, col, selected);
                    }
                    else if (state == eInsert) {
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
            }
            break;
    }

    return key_pressed;
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

static void drawAllFilled(Canvas &c, 
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

void Tui::Tui::setCursor() {
    static Screen::Cursor cursor;
    if (state == eInsert) {
        cursor.shape = Screen::Cursor::Shape::Block;
    }
    else if (state == ePencil) {
        cursor.shape = Screen::Cursor::Shape::Bar;
    }
    else {
        cursor.shape = Screen::Cursor::Shape::Hidden;
    }
    cursor.x = 4 + (col * 8);
    cursor.y = 2 + (row * 4);
    screen.SetCursor(cursor);
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

static void drawNextMove(Canvas &c, const Sudoku::logic::LogicalMove &move) {
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

static void drawPuzzleTable(Canvas &c) {
        const std::string toprow   = "╔═══════╤═══════╤═══════╦═══════╤═══════╤═══════╦═══════╤═══════╤═══════╗";
        const std::string rowtype1 = "║       │       │       ║       │       │       ║       │       │       ║";
        const std::string rowtype2 = "╟───────┼───────┼───────╫───────┼───────┼───────╫───────┼───────┼───────╢";
        const std::string rowtype3 = "╠═══════╪═══════╪═══════╬═══════╪═══════╪═══════╬═══════╪═══════╪═══════╣";
        const std::string botrow   = "╚═══════╧═══════╧═══════╩═══════╧═══════╧═══════╩═══════╧═══════╧═══════╝";

        auto style = [&] (Pixel &pixel) {
            pixel.bold = false;
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

void DrawPuzzle(Canvas &puzzleCanvas, const Sudoku::SudokuPuzzle &puzzle, char selected) {
    drawPuzzleTable(puzzleCanvas);
    drawAllPencils(puzzleCanvas, selected, puzzle.pencilMarks, puzzle.wrong_marks, puzzle.wrong_inputs);
    drawAllFilled(puzzleCanvas, selected, puzzle.constraintTable.current, puzzle.wrong_inputs, puzzle.current_start_index);
    if (puzzle.nextMove.type != Sudoku::logic::eMoveNotFound) {
        drawNextMove(puzzleCanvas, puzzle.nextMove);
    }
}

static Element sudokuFrame(Element inner, float focus) {
    return borderDouble(
        inner | vscroll_indicator | focusPositionRelative(0, focus) | yframe
    ) | size(WIDTH, EQUAL, 73) | size(HEIGHT, EQUAL, 37);
}

static Element helpElement(float focus_y) {
    FlexboxConfig inner;
    int width = 73;
    int height = 37;
    return sudokuFrame(
        vbox(
        text("Tuidoku, Sudoku for the terminal") | bold,
        separatorEmpty(),
        hbox(
        vbox(text("Keys") | bold,
        separatorLight(),
        hbox(text("0-9"), filler(), text("Insert or remove")),
        hbox(text("h"), filler(), text("Move left")),
        hbox(text("H"), filler(), text("Show hint")),
        hbox(text("i"), filler(), text("Insert mode")),
        hbox(text("j"), filler(), text("Move down")),
        hbox(text("k"), filler(), text("Move up")),
        hbox(text("l"), filler(), text("Move right")),
        hbox(text("p"), filler(), text("Pencil mode")),
        hbox(text("P"), filler(), text("Fill pencil marks")),
        hbox(text("q"), filler(), text("Quit")),
        hbox(text("?"), filler(), text("Show this page"))) | size(WIDTH, EQUAL, (width / 3) + 5),
        filler(),
        vbox(text("Mouse") | bold,
        separatorLight(),
        hbox(text("Click"), filler(), text("Move to cell")),
        hbox(text("Right-click"), filler(), text("Insert or pencil"))) | size(WIDTH, EQUAL, (width / 3) + 5)
        ),
        separatorEmpty(),
        text("Behaviour") | bold,
        separatorLight(),
        vbox({
            flexbox({text("Insert") | bold, filler(), paragraph(R"(When in insert mode the cursor should be block shaped. Pressing on a digit 0-9 in this mode inserts that digit in the cell if the cell is not a clue. Illogical inputs are highlighted, note that merely incorrect inputs will not be highlighted. Right-clicking on a cell when in input mode will insert the highlighted number into that cell if that number appears as a pencil-mark there.)"),}, inner),
            separatorEmpty(),
            flexbox({text("Pencil") | bold, filler(), paragraph(R"(When in pencil mode the cursor should be bar shaped. Pressing on a digit 1-9 in this mode will either mark or unmark that digit in the selected cell as long as that cell is empty. Illogical marks are highlighted, note that merely incorrect marks will not be highlighted. Right-clicking on an empty cell in this mode will either mark or unmark the highlighted number into that cell.)"),}, inner),
            separatorEmpty(),
            flexbox({text("Highlight") | bold, filler(), paragraph(R"(To select a number simply press the number key. To avoid changing the board unintentionally it is useful to insert on an underlined number or pencil on any filled cell. All occurences of that number will be highlighted.)"),}, inner),
            separatorEmpty(),
            flexbox({text("Mistakes") | bold, filler(), paragraph(R"(Pencil-marks or inputs that are illogical are shown. Illogical inputs or marks are inputs or marks that already appear in that row, column or box. Simply incorrect moves are not shown unless hints are visible.)"),}, inner),
            separatorEmpty(),
            flexbox({text("Hints") | bold, filler(), paragraph(R"(If There are any mistakes on the board they will be shown by showing hints. Pencil marks that are possible but that have yet to be added are also shown as hints. The green cells in a hint make up a truth, one or more of them must be true. None of the red cells in a hint can be true because of the relationship between the green cells.)")}, inner),
        })
    ) | borderEmpty, focus_y);
}
