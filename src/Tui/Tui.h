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
        SudokuPuzzle(Sudoku::DancingLinkTable *table);
        Sudoku::DancingLinkTable *constraintTable;
        size_t current_start_index;
        std::vector<Sudoku::DancingLink *> wrong_inputs;
        std::vector<Sudoku::DancingLink *> pencilMarks;   // contains visible marks
        std::vector<Sudoku::DancingLink *> wrong_marks;   // contains visible marks that are wrong
        std::vector<Sudoku::DancingLink *> removed_marks; // contains marks that have been visible but no longer are

        void insert(int row, int col, char num);
        void pencil(int row, int col, char num);
        void pencilAuto(int row, int col, char num);
    };

    class Board {
        private:
        enum stateEnum {
            eInsert,
            ePencil
        };

        stateEnum state;
        bool parseKeys(ftxui::Event event);

        bool autoPencil;

        SudokuPuzzle puzzle;

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