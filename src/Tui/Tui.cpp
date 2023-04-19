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

Tui::Board::Board(Sudoku::DancingLinkTable *table) :
    screen(ScreenInteractive::FitComponent()),
    row(0), col(0),
    puzzle(table),
    c(146, 148),
    state(eInsert){

    cursor.shape = Screen::Cursor::Shape::Block;

    renderer = Renderer([&] {
        const Canvas::Stylizer style_clues = [&](Pixel &pixel){
            pixel.underlined = true;
            pixel.bold = true;
        };

        const Canvas::Stylizer style_filled = [&](Pixel &pixel){
            pixel.bold = true;
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

        if (state == eInsert) {
            cursor.shape = Screen::Cursor::Shape::Block;
        }
        else {
            cursor.shape = Screen::Cursor::Shape::BlockBlinking;
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

    if (!key_pressed) {
        char pressed = event.character()[0];
        if ((pressed >= '0') && (pressed <= '9')) {
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

Tui::SudokuPuzzle::SudokuPuzzle(Sudoku::DancingLinkTable *table) : constraintTable(table) {
    current_start_index = constraintTable->current.size();
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
    char num_char[2] = {(char)(num + '0'), '\0'};

    int x = 8 + (col * 16);
    int y = 8 + (row * 16);

    c.DrawText(x, y, num_char, style);
}
