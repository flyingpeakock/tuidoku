#include "Game.h"
#include "File.h"
#include "Arguments.h"
#include "Generator.h"
#include "config.h"
#include <fstream>
#include <sstream>
#include <iostream>

void generate(int, bool, std::string);
void solve(bool, std::string);
void play(bool, std::string, int, bool);
void startCurses();
void endCurses();
WINDOW * createWindow();

int main(int argc, char *argv[]) {
    arguments args = arguments(argc, argv);
    if (args.printHelp()) {
        return 0;
    }
    if (args.shouldExit()) {
        return 1;
    }
    switch(args.getFeature()) {
        case feature::Generate:
        generate(args.getArgInt(), args.fileArgSet(), args.getFileName());
        break;
        case feature::Solve:
        solve(args.fileArgSet(), args.getFileName());
        break;
        case feature::Play:
        play(args.fileArgSet(), args.getFileName(), args.getArgInt(), args.bigBoard());
        break;
    }
    
}

void startCurses() {
    setlocale(LC_ALL, ""),
    initscr();
}

void endCurses() {
    endwin();
}

WINDOW * createWindow() {
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    WINDOW * ret = newwin(maxY, maxX, 0, 0);
    refresh();
    return ret;
}

Board makeNotSimpleBoard(SimpleBoard &board) {
    puzzle grid = board.getPlayGrid();
    std::stringstream gridStringStream;
    for (auto i = 0; i < grid.size(); i++) {
        for (auto j = 0; j < grid[i].size(); j++) {
            gridStringStream << grid[i][j];
        }
    }
    return Generator{gridStringStream.str().c_str()}.createBoard();
}

Board selectBoard(std::vector<SimpleBoard> boards) {
    if (boards.size() <= 1) {
        return makeNotSimpleBoard(boards[0]);
    }
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    SelectionWindow *win = new SelectionWindow(boards, createWindow());
    Selection game(win);
    int index = game.mainLoop();
    delete win;

    return makeNotSimpleBoard(boards[index]);
}


void generate(int empty, bool file, std::string fileName) {
    Generator gen = (empty) ? Generator(empty) : Generator();
    if (file) {
        std::ofstream fileStream;
        fileStream.open(fileName);
        gen.createBoard().printBoard(fileStream);
        return;
    }
    gen.createBoard().printBoard();
    return;
}

void solve(bool file, std::string fileName) {
    if (file) {
        selectBoard(file::getPuzzle(fileName.c_str())).printSolution();
        return;
    }
    else if (fileName != "404" && !fileName.empty()) {
        selectBoard(file::getStringPuzzle(fileName.c_str())).printSolution();
        return;
    }
    std::ostringstream gridString;
    for (auto i = 0; i < 81; i++) {
        gridString << '0';
    }
    Board b = Generator(gridString.str().c_str()).createBoard();;
    startCurses();
    SolveWindow window = SolveWindow(&b, createWindow());
    InteractiveSolver game(&window);
    game.mainLoop();
    endCurses();
    std::cout << "Puzzle:\n";
    b.printStart();
    std::cout << "Solution:\n";
    b.printSolution();
    std::cout << std::endl;
    return;
}

Board createBoard(bool file, std::string fileName, int empty) {
    if (file) {
        return selectBoard(file::getPuzzle(fileName.c_str()));
    }
    if (fileName != "404" && !fileName.empty()) {
        // no file attempting to get string board from fileName
        return selectBoard(file::getStringPuzzle(fileName.c_str()));
    }
    if (empty) {
        return Generator(empty).createBoard();
    }
    return Generator().createBoard();
}

void play(bool file, std::string fileName, int empty, bool big) {
    startCurses();
    Board b = createBoard(file, fileName, empty);
    Window *win = big ? new BigWindow(&b, createWindow()) : new Window(&b, createWindow());
    Game game(win, big);
    int playTime = game.mainLoop();
    delete win;
    endCurses();
    std::cout << "Puzzle:\n";
    b.printStart();
    std::cout << "\nSolution:\n";
    b.printSolution();
    if (playTime > 0) {
        std::cout << "\nSeconds played: " << playTime << std::endl;
    }

}