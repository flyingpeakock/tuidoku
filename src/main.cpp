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
void play(bool, std::string, int);

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
        play(args.fileArgSet(), args.getFileName(), args.getArgInt());
        break;
    }
    
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
        file::getPuzzle(fileName.c_str()).printSolution();
        return;
    }
    else if (fileName != "404" && !fileName.empty()) {
        file::getStringPuzzle(fileName.c_str()).printSolution();
        return;
    }
    std::ostringstream gridString;
    for (auto i = 0; i < 81; i++) {
        gridString << '0';
    }
    Board b = Generator(gridString.str().c_str()).createBoard();;
    SolveWindow window = SolveWindow(&b);
    InteractiveSolver game(&window);
    game.mainLoop();
    endwin();
    std::cout << "Puzzle:\n";
    b.printStart();
    std::cout << "Solution:\n";
    b.printSolution();
    std::cout << std::endl;
    return;
}

Board createBoard(bool file, std::string fileName, int empty) {
    if (file) {
        return file::getPuzzle(fileName.c_str());
    }
    if (fileName != "404" && !fileName.empty()) {
        // no file attempting to get string board from fileName
        return file::getStringPuzzle(fileName.c_str());
    }
    if (empty) {
        return Generator(empty).createBoard();
    }
    return Generator().createBoard();
}

void play(bool file, std::string fileName, int empty) {
    Board b = createBoard(file, fileName, empty);
    Window win = Window(&b);
    Game game(&win);
    game.mainLoop();
}