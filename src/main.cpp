#include "Game.h"
#include "File.h"
#include "Arguments.h"
#include "Generator.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "config.h"

void generate(int, bool, std::string);
void solve(bool, std::string);
void play(bool, std::string, int);

int main(int argc, char *argv[]) {

    std::map<std::string, bool> args = arguments::parse(argc, argv);
    if (args["help"]) {
        arguments::printHelp();
        return 0;
    }
    int argInt = 0;
    std::string argStr = arguments::getFileName(argc, argv);
    if (args["file"]) {
        if (argStr == "404") {
            std::cout << "No file name supplied.\n";
            return 1;
        }
    }
    if (args["empty"]) {
        argInt = arguments::getInt(argc, argv);
    }
    else if (args["filled"]) {
        argInt = 81 - arguments::getInt(argc, argv);
    }

    if ((args["empty"] || args["filled"]) && argInt == 0) {
        std::cout << "No number supplied.\n";
        return 1;
    }


    if (arguments::incompatible(args))
        return 1;

    if (args["generate"]) {
        generate(argInt, args["file"], argStr);
        return 0;
    }
    if (args["solve"]) {
        solve(args["file"], argStr);
        return 0;
    }
    if (args["play"]) {
        play(args["file"], argStr, argInt);
        return 0;
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