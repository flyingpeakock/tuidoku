#include <iostream>                     // for operator<<, basic_ostream, cout
#include <vector>                       // for vector

#include "Tui/Tui.h"                    // for Tui
#include "sudoku/DancingLinkObjects.h"  // for eSize, DancingLinkContainer

void printPuzzle(sudoku::DancingLinkTable *table) {
    int array[sudoku::eSize][sudoku::eSize] = {0};

    sudoku::DancingLinkContainer innertable = table->solution;
    int index = 0;
    for (auto &current : innertable) {
        int count = current->count;
        int i = count / (sudoku::eBoardSize);
        int j = (count % sudoku::eBoardSize) / sudoku::eSize;
        int num = (count % sudoku::eSize) + 1;
        array[i][j] = num;
    }

    char text[sudoku::eBoardSize + 1] = {'\0'};
    for (auto i = 0; i < sudoku::eSize; i++) {
        for (auto j = 0; j < sudoku::eSize; j++) {
            text[(i * sudoku::eSize) + j] = array[i][j] + '0';
        }
    }
    std::cout << (const char *)text << '\n';
}

int main(int argc, char *argv[]) {
    //Tui::Board(sudoku::generate());
    //Tui::test();
    /*
    for (auto i = 0; i < 100; i++) {
        //sudoku::DancingLinkTable table(true);
        //sudoku::solve(&table, true);
        auto table = sudoku::generate();
        printPuzzle(&table);
    }
    Conf::init();
    Tui::up_key = Conf::keyBind("up");
    Tui::down_key = Conf::keyBind("down");
    Tui::select_key = '\n';
    */
    //auto table = sudoku::generate("240030001590010320000020004352146897400389512189572643020093100600051009900060030"); // triple
    //auto table = sudoku::generate(sudoku::eAny);
    //auto table = sudoku::generate(sudoku::eHard);

    tui::Tui tui;
    tui.runLoop();
    return 0;
}