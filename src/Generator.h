#pragma once
#include "Board.h"

class Generator {
    private:
        puzzle grid;
        puzzle solution;
        void generate(int unknown);
        void generateGiveEmpty(int unknowns);

    public:
        Generator();
        Generator(int unknowns);
        Generator(const char *gridString);
        puzzle getGrid();
        puzzle getSolution();
        Board createBoard();
};