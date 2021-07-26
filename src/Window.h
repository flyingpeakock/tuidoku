#pragma once

#include "Board.h"
#include <ncurses.h>
#include <string>
#include "Row.h"
#include <map>

class BasicWindow {
    private:
        void init();
    protected:
        WINDOW *window;
        SimpleBoard *game;
        int cursorRow, cursorCol;
        int windowRows, windowCols;
        int BoardRows;
        int BoardCols;
        int boardTop;
        int boardLeft;
        virtual void printBoxes();
        virtual void printNumbs();
        virtual void printCoords();
        virtual void printCursor();
        void clear();
        bool resize();
        virtual void printInstructions() = 0;
        virtual int getColor(char c, int row, int col);
    
    public:
        BasicWindow(SimpleBoard *g);
        BasicWindow(SimpleBoard *g, WINDOW *w);
        ~BasicWindow();

        Board *getBoard();
        virtual void printBoard();
        void moveCursor(int row, int col);
};

class SolveWindow : public BasicWindow {
    private:
        Board *game;
        void printInstructions();
    public:
        SolveWindow(Board *g);
        SolveWindow(Board *g, WINDOW *w);
};

class Window : public BasicWindow {
    protected:
        Board *game;
        int highlightNum;
        std::string mode;
        bool checkColors;

        void printInstructions();

        int getColor(char c, int row, int col);
        virtual void printPencil();
        void printMode();

    public:
        Window(Board *g);
        Window(Board *g, WINDOW *w);
        virtual void printBoard();
        void changeMode(std::string s);
        void check();
        void select(int val);
};

class BigWindow : public Window {
    private:
        void clearPencil(int row, int col);
        void printPencil();
        void printPencil(char c, int row, int col, std::map<char, bool> marks);
        void printBoxes();
        void printNumbs();
        void printCursor();
        void printCoords();
    public:
        BigWindow(Board *g);
        BigWindow(Board *g, WINDOW *w);
        void printBoard();
};

class SelectionWindow : public BasicWindow {
    private:
        std::vector<SimpleBoard> boardVector;
        void printInstructions();
    public:
        SelectionWindow(std::vector<SimpleBoard> &g);
        SelectionWindow(std::vector<SimpleBoard> &g, WINDOW *w);
        void changeBoard(int idx);
        void printBoard();
};