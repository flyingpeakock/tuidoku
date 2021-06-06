#pragma once

#include "Window.h"
#include "Board.h"

class Game{
    private:
        Board board;
        Window window;
        wchar_t mode;
        int row, col;
    public:
        Game(Board b);
        void mainLoop();
        void changeMode(char c);
        void up();
        void down();
        void left();
        void right();
        void insert(char val);
        void pencil(char val);
        void go();
        void check();
};