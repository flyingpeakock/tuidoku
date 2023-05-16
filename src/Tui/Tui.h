#ifndef TUI_H
#define TUI_H

#include <ftxui/component/screen_interactive.hpp>  // for ScreenInteractive
#include <ftxui/component/component.hpp>           // for ComponentDecorator
#include <ftxui/component/component_base.hpp>      // for Component
#include <future>                                  // for future

#include "sudoku/Constants.h"                      // for difficulty
#include "sudoku/DancingLinkObjects.h"             // for DancingLinkTable
#include "sudoku/SudokuPuzzle.h"                   // for SudokuPuzzle

namespace ftxui {
struct Event;
}  // namespace ftxui

namespace tui {
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
        sudoku::difficulty difficulty;
        stateEnum state;
        bool generating_puzzle;

        ftxui::ScreenInteractive screen;

        /* sudoku Puzzle */
        std::future<sudoku::DancingLinkTable> table_promise;
        sudoku::DancingLinkTable table;
        sudoku::SudokuPuzzle puzzle;

        /* Parsers */
        ftxui::ComponentDecorator parseInput;
        void parseMenuChoice(int choice);
        bool parseEvent(ftxui::Event event, sudoku::SudokuPuzzle &puzzle);


        /* Renderers*/
        ftxui::Component board_renderer;
        ftxui::Component help_renderer;
        void setCursor();
    };
}

#endif // TUI_H
