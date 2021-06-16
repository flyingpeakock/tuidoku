#pragma once

#include "Board.h"
#include <ncurses.h>
#include <string>
#include "Row.h"

class BasicWindow {
    private:
        void init();
    protected:
        WINDOW *window;
        Board *game;
        int cursorRow, cursorCol;
        int windowRows, windowCols;
        const int BoardRows = 19;
        const int BoardCols = 37;
        int boardTop;
        int boardLeft;
        void printBoxes();
        void printNumbs();
        void printCoords();
        void printCursor();
        void clear();
        virtual void printInstructions() = 0;
    
    public:
        BasicWindow(Board *g);
        BasicWindow(Board *g, WINDOW *w);
        ~BasicWindow();

        Board *getBoard();
        void printBoard();
        void moveCursor(int row, int col);
};

class SolveWindow : public BasicWindow {
    private:
        void printInstructions();
    public:
        SolveWindow(Board *g);
        SolveWindow(Board *g, WINDOW *w);
};

class Window : public BasicWindow {
    private:
        std::string mode;
        bool checkColors;
        int highlightNum;

        void printNumbs();
        void printInstructions();

        int getColor(char c, int row, int col);
        void printPencil();
        void printMode();

    public:
        Window(Board *g);
        Window(Board *g, WINDOW *w);
        void printBoard();
        void changeMode(std::string s);
        void check();
        void select(int val);
};