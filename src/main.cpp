#include "Game.h"
#include "File.h"
#include "Arguments.h"
#include "Generator.h"
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include "config.h"

void printHelp() {
    const char *helptext = "usage: tuidoku [OPTIONS] \n\n"
                           "Play Sudoku in the terminal.\n\n"
                           "Optional args:\n"
                           "-h, --help       View this page\n"
                           "-w,  --wasd      Display wasd instead of hjkl\n"
                           "--opensudoku     Load a puzzle from an opensudoku file\n"
                           "--sdm            Load a puzzle from an smd file\n"
                           "INTEGER          Number of squares to leave empty\n\n"
                           "Move with hjkl, wasd or the arrow keys.\n"
                           "Enter pencil mode by pressing 'p'.\n"
                           "Enter insert mode by pressing 'i'.\n"
                           "Go to a specific square by pressing 'g' followed by the column then row.\n"
                           "Check the board for mistakes by pressing 'c'.\n"
                           "Quit the game by pressing 'q'.\n"
                           "To highlight any occurence of a number insert the number in any mode or\n"
                           "press spacebar on a prefilled box.\n"
                           "Hint: pencilmarks do not show up on filled in boxes, use this to highlight.\n\n"
                           "For information on Sudoku puzzles see the wikipedia entry:\n"
                           "https://en.wikipedia.org/wiki/Sudoku\n\n"
                           "See the readme for more information.\n"
                           "The readme can be read online at:\n"
                           "https://github.com/flyingpeakock/tuidoku/blob/master/README.md\n";
    printf(helptext);
}


int main(int argc, char *argv[]) {

    std::map<std::string, bool> args = arguments::parse(argc, argv);
    int argInt = 0;
    std::string argStr;
    if (args["file"]) {
        argStr = arguments::getFileName(argc, argv);
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

    if ((args["empty"] || args["filled"]) && !argInt) {
        std::cout << "No number supplied.\n";
        return 1;
    }

    if (args["help"]) {
        arguments::printHelp();
        return 0;
    }

    if (arguments::incompatible(args))
        return 1;


    if (args["generate"]) {
        Generator gen = (args["filled"] || args["empty"]) ? Generator(argInt) : Generator();
        if (args["file"]) {
            std::ofstream fileStream;
            fileStream.open(argStr);
            gen.createBoard().printBoard(fileStream);
            return 0;
        }
        gen.createBoard().printBoard();
        return 0;
    }

    if (args["solve"]) {
        if (args["file"]) {
            Board b = file::getPuzzle(argStr.c_str());
            b.printSolution();
        }
        else if (!argStr.empty()) {
            // Assume that the puzzle is argStr
            file::getStringPuzzle(argStr.c_str()).printSolution();
        }
        else {
            std::cout << "TODO: create a way to interactively input puzzle";
        }
        return 0;
    }


    if (args["play"]) {
        Board b = args["file"] ? file::getPuzzle(argStr.c_str()) : (!argStr.empty() ? file::getStringPuzzle(argStr.c_str()) : (args["empty"] || args["filled"]) ? Generator(argInt).createBoard() : Generator().createBoard());
        Game game(b);
        game.mainLoop();
        return 0;
    }
    
}
