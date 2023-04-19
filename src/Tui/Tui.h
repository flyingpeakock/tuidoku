#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/component.hpp>
#include "../Sudoku/Sudoku.h"

namespace Tui {
    struct SudokuPuzzle {
        Sudoku::DancingLinkTable *constraintTable;
        std::vector<Sudoku::DancingLink *> clues;

        SudokuPuzzle(Sudoku::DancingLinkTable *table);
    };

    class Board {
        private:
        SudokuPuzzle puzzle;

        ftxui::ScreenInteractive screen;
        ftxui::Screen::Cursor cursor;
        int row;
        int col;

        public:
        Board(Sudoku::DancingLinkTable *constraintTable);
        ftxui::Component renderer;
        ftxui::Component keypress;
    };
}