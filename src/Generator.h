#pragma once
#include "Solver.h"
#include "Board.h"

class Generator {
    private:
        std::array<std::array<int, 9>, 9> grid;
        std::array<std::array<int, 9>, 9> solution;
        void generate(int unknown);
        void generateGiveEmpty(int unknowns);

    public:
        Generator();
        Generator(int unknowns);
        Generator(const char *gridString);
        std::array<std::array<int, 9>, 9> getGrid();
        std::array<std::array<int, 9>, 9> getSolution();
        Board createBoard();
};