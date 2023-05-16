#include "Tui.h"
#include <chrono>
#include <ctime>
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
static void drawAllFilled(Canvas &c, 
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
static void drawNextMove(Canvas &c, const Sudoku::logic::LogicalMove &move);

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

/*
Tui::PuzzleCanvas::PuzzleCanvas() :
    Canvas(146, 148),
    selected(0), row(0), col(0)
{
}
*/

void DrawPuzzle(Canvas &puzzleCanvas, const Sudoku::SudokuPuzzle &puzzle, char selected) {
    drawPuzzleTable(puzzleCanvas);
    drawAllPencils(puzzleCanvas, selected, puzzle.pencilMarks, puzzle.wrong_marks, puzzle.wrong_inputs);
    drawAllFilled(puzzleCanvas, selected, puzzle.constraintTable.current, puzzle.wrong_inputs, puzzle.current_start_index);
    if (puzzle.nextMove.type != Sudoku::logic::eMoveNotFound) {
        drawNextMove(puzzleCanvas, puzzle.nextMove);
    }
}

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

bool parseKeys(Event event, int &row, int &col) {
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

/*
ftxui::Element Tui::PuzzleCanvas::getCanvas() {
    return canvas(static_cast<ftxui::Canvas>(*this));
}
*/

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
    focus_y(0.0f)
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
            e = e | bold | bgcolor(Color::DarkOrange);
        }
        return e;
    };

    /* Setting menu behaviour*/
    option.on_enter = [&]{
        parseMenuChoice(choice);
    };

    /* Board renderer */
    auto board_renderer = Renderer([&]{
        Canvas puzzleCanvas(146, 148);
        DrawPuzzle(puzzleCanvas, puzzle, selected);
        return canvas(std::move(puzzleCanvas));
    });

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

    /* Help text renderer */
    auto help_renderer = Renderer([&]{
        FlexboxConfig outer, inner;
        int width = 73;
        int height = 37;
        return
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
        ) | borderEmpty | vscroll_indicator | focusPositionRelative(0, focus_y) | yframe | borderDouble | size(WIDTH, EQUAL, width) | size(HEIGHT, EQUAL, height) ;
    });

    /* Parse input for everything */
    parseInput = CatchEvent([&](Event event){
        if (state == eMenu) {
            if (event == Event::Character('q')) {
                selected = 0;
                puzzle.nextMove.type = Sudoku::logic::eMoveNotFound;
                state = eExit;
                return true;
            }
            if (generating_puzzle) {
                using namespace std::chrono_literals;
                auto status = table_promise.wait_for(0ms);
                if (status == std::future_status::ready) {
                    table = table_promise.get();
                    puzzle = Sudoku::SudokuPuzzle(table);
                    state = eInsert;
                    generating_puzzle = false;
                }
            }
            if (event != Event::Character('?')) {
                return generating_puzzle; // Use defualt menu keys
            }
        }
        if (table.root->right == table.root.get()) {
            state = eMenu;
            selected = 0;
            puzzle.nextMove.type = Sudoku::logic::eMoveNotFound;
            return true;
        }
        return parseEvent(event, puzzle);
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
            Screen::Cursor cursor;
            cursor.shape = Screen::Cursor::Hidden;
            screen.SetCursor(cursor);
        }
        else {
            tab_drawn = 0;
            setCursor(screen, row, col, state == ePencil);
        }

        if (state == eExit) {
            screen.ExitLoopClosure()();
        }

        return container->Render();
    });

    //screen.Loop(renderer);
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
    if (state == eHelp) {
        if (event.is_character()) {
            if (event == Event::Character('k')) {
                focus_y -= 0.1f;
                if (focus_y > 0.7f) { focus_y = 0.6f; }
            }
            else if (event == Event::Character('j')) {
                focus_y += 0.1f;
                if (focus_y < 0.3f) {focus_y = 0.4f; };
            }
            else {
                focus_y = 0.0f;
                state = previousState;
            }
            return true;
        }
        if (event.is_mouse()) {
            auto m = event.mouse();
            if (m.button != Mouse::Button::None && m.motion == Mouse::Motion::Pressed) {
                state = previousState;
                return true;
            }
        }
        return false;
    }
    if (event.is_character()) {
        if (parseKeys(event, row, col)) {
            key_pressed = true;
        }
        else if (event == Event::Character("q")) {
            selected = 0;
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
        else if (event == Event::Character("?")) {
            if (state == eHelp) {
                state = previousState;
            }
            else {
                previousState = state;
                state = eHelp;
            }
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
    }
    else if (event.is_mouse()) {
        key_pressed = parseMouse(event, row, col);
        if (key_pressed && event.mouse().button == Mouse::Button::Left) {

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
