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
        stateEnum state;

        bool parseKeys(ftxui::Event event);
        bool parseMouse(ftxui::Event event);

        Sudoku::SudokuPuzzle puzzle;

        ftxui::ScreenInteractive screen;
        ftxui::Screen::Cursor cursor;
        ftxui::ComponentDecorator parseEvent;
        ftxui::Canvas c;
        int row;
        int col;
        char selected;

        public:
        Board(Sudoku::DancingLinkTable *constraintTable);
        void playLoop();
        ftxui::Component renderer;
    };
}