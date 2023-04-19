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

static void drawFilledCells(Canvas &c, const std::vector<Sudoku::DancingLink *> &cells, const Canvas::Stylizer &style);

Tui::Board::Board(Sudoku::DancingLinkTable *table) :
    screen(ScreenInteractive::FitComponent()),
    row(0), col(0),
    puzzle(table){
    cursor.shape = Screen::Cursor::Shape::Block;

    renderer = Renderer([&] {
        auto c = Canvas(146, 148);

        const Canvas::Stylizer style_clues = [&](Pixel &pixel){
            pixel.underlined = true;
            pixel.bold = true;
        };

        const Canvas::Stylizer style_filled = [&](Pixel &pixel){
            pixel.bold = true;
        };

        drawPuzzleTable(c);
        drawFilledCells(c, puzzle.clues, style_clues);

        cursor.x = (8 + (col * 16)) / 2;
        cursor.y = (8 + (row * 16)) / 4;
        screen.SetCursor(cursor);

        return frame(canvas(std::move(c)));
    });

    auto keypress = CatchEvent(renderer, [&](Event event) {
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
    });
    screen.Loop(keypress);
}

Tui::SudokuPuzzle::SudokuPuzzle(Sudoku::DancingLinkTable *table) : constraintTable(table), clues(table->current) {
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

void drawFilledCells(Canvas &c, const std::vector<Sudoku::DancingLink *> &cells, const Canvas::Stylizer &style) {
    for (const auto &cell : cells) {
        int row = Sudoku::getRowFromLink(cell);
        int col = Sudoku::getColFromLink(cell);
        int num = Sudoku::getNumFromLink(cell);
        char num_char[2] = {(char)(num + '0'), '\0'};

        int x = 8 + (col * 16);
        int y = 8 + (row * 16);

        c.DrawText(x, y, num_char, style);
    }
}
