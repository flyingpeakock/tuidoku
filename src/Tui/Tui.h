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

    /*
    struct PuzzleCanvas : public ftxui::Canvas {
        PuzzleCanvas();
        void DrawPuzzle(const Sudoku::SudokuPuzzle &puzzle);

        ftxui::Element getCanvas();
        bool parseMouse(ftxui::Event event);
        bool parseKeys(ftxui::Event event);

        int row;
        int col;
        char selected;
        bool showNextMove;
    };
    */

    class Tui {
        public:
        Tui();
        void runLoop();

        private:
        int row;
        int col;
        char selected;
        float focus_y;

        Sudoku::difficulty difficulty;
        stateEnum state;
        ftxui::ScreenInteractive screen;
        //PuzzleCanvas puzzleCanvas;
        bool generating_puzzle;

        std::future<Sudoku::DancingLinkTable> table_promise;
        Sudoku::DancingLinkTable table;
        Sudoku::SudokuPuzzle puzzle;

        ftxui::ComponentDecorator parseInput;
        void parseMenuChoice(int choice);

        bool parseEvent(ftxui::Event event, Sudoku::SudokuPuzzle &puzzle);
    };
}

#endif // TUI_H
