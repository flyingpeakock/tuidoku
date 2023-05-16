#ifndef TUI_H
#define TUI_H

#include <ftxui/component/screen_interactive.hpp>  // for ScreenInteractive
#include <ftxui/component/component.hpp>           // for ComponentDecorator
#include <ftxui/component/component_base.hpp>      // for Component
#include <future>                                  // for future

#include "Sudoku/Constants.h"                      // for difficulty
#include "Sudoku/DancingLinkObjects.h"             // for DancingLinkTable
#include "Sudoku/SudokuPuzzle.h"                   // for SudokuPuzzle

namespace ftxui {
struct Event;
}  // namespace ftxui

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
