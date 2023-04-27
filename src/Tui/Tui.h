#pragma once

#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include "../Sudoku/Sudoku.h"
#include "../Sudoku/Logic.h"

namespace Tui {
    class Board {
        private:

        enum stateEnum {
            eInsert,
            ePencil,
        };
        /** state of the board, pencil or insert */
        stateEnum state;

        /**
         * @brief Parse the key pressed
         *
         * @param event ftxui::Event
         * @return true if event was handled
         * @return false if event was not handled
         */
        bool parseKeys(ftxui::Event event);

        /**
         * @brief Parse the mouse key pressed
         * 
         * @param event ftxui::Event
         * @return true if the event was handled
         * @return false if the event was not handled
         */
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
        std::vector<Sudoku::logic::Move> moves;

        bool showNextMove;

        public:
        /** constructor with constraintTable representing puzzle*/
        Board(Sudoku::DancingLinkTable *constraintTable);

        /** main loop*/
        void playLoop();
    };
}