#pragma once

#include "Window.h"
#include "Board.h"

class Controller {
    protected:
        Board board;
        BasicWindow window;
        int row, col;
    public:
        Controller(Board b);
        virtual void mainLoop();
        virtual void up();
        virtual void down();
        virtual void left();
        virtual void right();
        virtual void insert(char val);
        virtual void go();
};

class Game : public Controller {
    private:
        Window window;
        wchar_t mode;
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