#pragma once

#include "Board.h"
#include <ncurses.h>
#include <string>

class BasicWindow {
    protected:
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
        virtual void printInstructions();
    
    public:
        BasicWindow(Board *g);
        ~BasicWindow();

        Board *getBoard();
        void printBoard();
        void moveCursor(int row, int col);
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
        void printBoard();
        void changeMode(std::string s);
        void check();
        void select(int val);
};