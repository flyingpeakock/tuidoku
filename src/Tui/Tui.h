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
        //std::vector<Sudoku::DancingLink *> clues;
        SudokuPuzzle(Sudoku::DancingLinkTable *table);
        size_t current_start_index;
    };

    class Board {
        private:
        enum stateEnum {
            eInsert,
            ePencil
        };

        stateEnum state;
        bool parseKeys(ftxui::Event event);

        SudokuPuzzle puzzle;

        ftxui::ScreenInteractive screen;
        ftxui::Screen::Cursor cursor;
        ftxui::ComponentDecorator parseEvent;
        ftxui::Canvas c;
        int row;
        int col;

        public:
        Board(Sudoku::DancingLinkTable *constraintTable);
        void playLoop();
        ftxui::Component renderer;
    };
}