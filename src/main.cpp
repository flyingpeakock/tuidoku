#include "Sudoku/Sudoku.h"
#include "Tui/Tui.h"
#include <iostream>

void printPuzzle(Sudoku::DancingLinkTable *table) {
    int array[Sudoku::eSize][Sudoku::eSize] = {0};

    Sudoku::DancingLinkContainer innertable = table->solution;
    int index = 0;
    for (auto &current : innertable) {
        int count = current->count;
        int i = count / (Sudoku::eBoardSize);
        int j = (count % Sudoku::eBoardSize) / Sudoku::eSize;
        int num = (count % Sudoku::eSize) + 1;
        array[i][j] = num;
    }

    char text[Sudoku::eBoardSize + 1] = {'\0'};
    for (auto i = 0; i < Sudoku::eSize; i++) {
        for (auto j = 0; j < Sudoku::eSize; j++) {
            text[(i * Sudoku::eSize) + j] = array[i][j] + '0';
        }
    }
    std::cout << (const char *)text << '\n';
}

int main(int argc, char *argv[]) {
    //Tui::Board(Sudoku::generate());
    //Tui::test();
    /*
    for (auto i = 0; i < 100; i++) {
        //Sudoku::DancingLinkTable table(true);
        //Sudoku::solve(&table, true);
        auto table = Sudoku::generate();
        printPuzzle(&table);
    }
    Conf::init();
    Tui::up_key = Conf::keyBind("up");
    Tui::down_key = Conf::keyBind("down");
    Tui::select_key = '\n';
    */
    while (true) {
        //auto table = Sudoku::generate(Sudoku::eAny);
        auto table = Sudoku::generate(Sudoku::eBeginner);
        Tui::Board board(table);
        board.playLoop();
    }
    return 0;
}