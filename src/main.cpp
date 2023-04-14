#include "Sudoku/Sudoku.h"
#include <iostream>

void printPuzzle(Sudoku::DancingLinkTable *table) {
    int array[Sudoku::eSize][Sudoku::eSize] = {0};

    Sudoku::DancingLink *current = table->current[0];
    int index = 0;
    //for (Sudoku::DancingLink **current = table->solution; current != nullptr; current++) {
    while (current) {
        int count = current->count;
        int i = count / (Sudoku::eBoardSize);
        int j = (count % Sudoku::eBoardSize) / Sudoku::eSize;
        int num = (count % Sudoku::eSize) + 1;
        array[i][j] = num;
        current = table->current[++index];
    }

    for (auto i = 0; i < Sudoku::eSize; i++) {
        for (auto j = 0; j < Sudoku::eSize; j++) {
            std::cout << array[i][j] << "   ";
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

int main(int argc, char *argv[]) {
    for (auto i = 0; i < 100; i++) {
        //Sudoku::DancingLinkTable table(true);
        //Sudoku::generate(&table);
        //Sudoku::solve(&table, true);
        auto table = Sudoku::generate();
        printPuzzle(&table);
    }
    /*
    Conf::init();
    Tui::up_key = Conf::keyBind("up");
    Tui::down_key = Conf::keyBind("down");
    Tui::select_key = '\n';
    */
    
    return 0;
}