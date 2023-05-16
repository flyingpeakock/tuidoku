#ifndef TUI_H
#define TUI_H

#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <Sudoku/Sudoku.h>
#include <Sudoku/Logic.h>
#include <future>

namespace Tui {
    enum stateEnum {
        eInsert,
        ePencil,
        eMenu,
        eExit,
        eHelp,
    };

    class Tui {
        public:
        Tui();
        void runLoop();

        private:

        /* States */
        int row;
        int col;
        char selected;
        float focus_y;
        Sudoku::difficulty difficulty;
        stateEnum state;
        bool generating_puzzle;

        ftxui::ScreenInteractive screen;

        /* Sudoku Puzzle */
        std::future<Sudoku::DancingLinkTable> table_promise;
        Sudoku::DancingLinkTable table;
        Sudoku::SudokuPuzzle puzzle;

        /* Parsers */
        ftxui::ComponentDecorator parseInput;
        void parseMenuChoice(int choice);
        bool parseEvent(ftxui::Event event, Sudoku::SudokuPuzzle &puzzle);


        /* Renderers*/
        ftxui::Component board_renderer;
        ftxui::Component help_renderer;
        void setCursor();
    };
}

#endif // TUI_H
