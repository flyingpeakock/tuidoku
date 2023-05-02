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
    //auto table = Sudoku::generate("400270600798156234020840007237468951849531726561792843082015479070024300004087002"); // Naked double
    //auto table = Sudoku::generate("801006094300009080970080500547062030632000050198375246083620915065198000219500008"); // Hidden double
    //auto table = Sudoku::generate("009032000000700000162000000010020560000900000050000107000000403026009000005870000"); // Hidden double
    //auto table = Sudoku::generate("458000793693500214700493685005900006040035070300002450060100007004009060100050840"); // x-wing
    //auto table = Sudoku::generate("240030001590010320000020004352146897400389512189572643020093100600051009900060030"); // triple
    //auto table = Sudoku::generate(Sudoku::eAny);
    //auto table = Sudoku::generate(Sudoku::eHard);
    std::vector<std::string> tables = {
        "458000793693500214700493685005900006040035070300002450060100007004009060100050840",
        "240030001590010320000020004352146897400389512189572643020093100600051009900060030",
        "570004060004000352009000040050096410000407000400105000168042093043000000005013084",
        "529410703006003002003200000052300076637050200190627530300069420200830600960742305"};
    for (auto &string: tables) {
        auto table = Sudoku::generate(string);
        Tui::Board board(table);
        board.playLoop();
    }
    auto table = Sudoku::generate(Sudoku::eAny);
    Tui::Board board(table);
    board.playLoop();
    return 0;
}