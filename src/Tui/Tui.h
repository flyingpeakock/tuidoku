#pragma once

#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include "../Sudoku/Sudoku.h"

namespace Tui {
    class Board {
        private:

        enum stateEnum {
            eInsert,
            ePencil,
        };
        /** state of the board, pencil or insert */
        stateEnum state;

        /** function to parse keys pressed */
        bool parseKeys(ftxui::Event event);

        /** function to parse mouse clicked */
        bool parseMouse(ftxui::Event event);

        /** suboku puzzle object */
        Sudoku::SudokuPuzzle puzzle;

        /** ftxui objects*/
        ftxui::ScreenInteractive screen;
        ftxui::Canvas c;
        ftxui::ComponentDecorator parseEvent;
        ftxui::Component renderer;

        /** puzzle state */
        int row;
        int col;
        char selected;

        public:
        /** constructor with constraintTable represent puzzle*/
        Board(Sudoku::DancingLinkTable *constraintTable);

        /** main loop*/
        void playLoop();
    };
}