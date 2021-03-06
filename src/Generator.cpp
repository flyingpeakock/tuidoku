#include "Generator.h"
#include "Solver.h"
#include <array>
#include <algorithm>
#include <random>
#include <chrono>
#include <string.h>
#include <stdexcept>

#define SIZE 9

Generator::Generator(){
    generate(0);
};

Generator::Generator(const char *gridString) {
    if (strlen(gridString) != 81)
        throw std::invalid_argument("Wrong amount of digits for a sudoku puzzle");
    
    auto idx = 0;
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            grid[i].set(j, gridString[idx++] - '0');
        }
    }
}

Generator::Generator(int unknowns) {
    if (unknowns > 64)
        unknowns = 64;
    generateGiveEmpty(unknowns);
}

void Generator::generateGiveEmpty(int unknowns) {
    generate(unknowns);
    int empty = 0;
    for (auto i = 0; i < grid.size(); i++) {
        for (auto j = 0; j < grid[i].size(); j++) {
            if (grid[i][j] == 0)
                empty++;
        }
    }
    if (empty != unknowns)
        generateGiveEmpty(unknowns);
}

void Generator::generate(int unknown) {
    struct Cell
    {
        int row;
        int col;
        int val;
    };
    

    Solver solver = Solver();
    solver.solve();
    grid = solver.getGrid();

    std::array<Cell, SIZE*SIZE> cells;
    int count = 0;
    for (auto i = 0; i < SIZE; i++) {
        for (auto j = 0; j < SIZE; j++) {
            cells[count].row = i;
            cells[count].col = j;
            cells[count].val = grid[i][j];
            count++;
        }
    }

    // Shuffle array to randomly remove positions
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (cells.begin(), cells.end(), std::default_random_engine(seed));

    int unknowns = 0;
    for (auto cell: cells) {
        grid[cell.row].set(cell.col, 0);
        solver.changeGrid(grid);
        solver.solve();
        if (!solver.isUnique()) {
            // Removal of value does not produce unique solution
            // Put it back
            grid[cell.row].set(cell.col, cell.val);
        }
        else {
            unknowns++;
        }
        if (unknown && unknowns >= unknown)
            break;
    }

    // Solve again incase last removal broke the puzzle
    solver.changeGrid(grid);
    solver.solve();
    solution = solver.getGrid();

}


puzzle Generator::getGrid() {
    return grid;
}

puzzle Generator::getSolution() {
    return solution;
}

Board Generator::createBoard() {
    if (!solution[0][0]) {
        Solver solver(grid);
        solver.solve();
        solution = solver.getGrid();
    }
    return Board(grid, solution);
}

SimpleBoard Generator::createSimpleBoard() {
    return SimpleBoard(grid);
}