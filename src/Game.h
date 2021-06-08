#pragma once

#include "Window.h"
#include "Board.h"

class Controller {
    protected:
        Board *board;
        BasicWindow *window;
        int row : 5;
        int col : 5;
    public:
        Controller(BasicWindow *win);
        virtual void mainLoop();
        void up();
        void down();
        void left();
        void right();
        void insert(char val);
        void go();
};

class Game : public Controller {
    private:
        Window *window;
        wchar_t mode;
    public:
        Game(Window *win);
        void mainLoop();
        void changeMode(char c);
        void pencil(char val);
        void check();
};