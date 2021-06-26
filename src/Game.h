#pragma once

#include "Window.h"
#include "Board.h"
#include "Solver.h"

class Controller {
    protected:
        Board *board;
        BasicWindow *window;
        int row : 5;
        int col : 5;
    public:
        Controller(BasicWindow *win);
        virtual int mainLoop();
        void up();
        void down();
        void left();
        void right();
        void go();
};

class InteractiveSolver : public Controller {
    private:
        SolveWindow *window;
        Solver solver;
        void solve();
    public:
        InteractiveSolver(SolveWindow *win);
        int mainLoop();
};

class Game : public Controller {
    private:
        Window *window;
        wchar_t mode;
    public:
        Game(Window *win);
        int mainLoop();
        void changeMode(char c);
        void check();
};