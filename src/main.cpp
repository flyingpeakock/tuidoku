#include "Game.h"
#include "File.h"
#include "Generator.h"
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>

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
    /*Generator *gen = nullptr;
    /*for (auto i = 1; i < argc; i++) {
        if (atoi(argv[i]) > 0) {
            gen = new Generator(atoi(argv[i]));
        }
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printHelp();
            return 0;
        }
        if (strcmp(argv[i], "--opensudoku") == 0 && !gen) {
            gen = new Generator(file::getRandomXMLPuzzle(argv[i+1]).c_str());
        }
        if (strcmp(argv[i], "--sdm") == 0 && !gen) {
            gen = new Generator(file::getRandomSDMPuzzle(argv[i+1]).c_str());
        }
    }
    if (!gen)
        gen = new Generator(64);
    gen->createBoard().printBoard();
    /*Game game(board);
    game.mainLoop();
    delete gen;*/
    bool help = false;
    bool play = false;
    bool generate = false;
    bool solve = false;
    bool file = false;
    bool empty = false;
    bool filled = false;
    char *argStr;
    int argInt = 0;
    for (auto i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] != '-') {
            // Single letter arguments, loop through every letter
            for (auto j = 1; j < strlen(argv[i]); j++) {
                char c = argv[i][j];
                switch(c) {
                    case 'h':
                    help = true;
                    break;
                    case 'p':
                    play = true;
                    break;
                    case 'g':
                    generate = true;
                    break;
                    case 's':
                    solve = true;
                    break;
                    case 'f':
                    file = true;
                    break;
                    case 'e':
                    empty = true;
                    break;
                    case 'F':
                    filled = true;
                }
            }
        }
        else if (argv[i][0] == '-' && argv[i][1] == '-') {
            // Long form arguments
            if (strcmp(argv[i]+2, "help")) {
                help = true;
            }
            else if (strcmp(argv[i]+2, "play")) {
                play = true;
            }
            else if (strcmp(argv[i]+2, "generate")) {
                generate = true;
            }
            else if (strcmp(argv[i]+2, "solve")) {
                solve = true;
            }
            else if (strcmp(argv[i]+2, "file")) {
                file = true;
            }
            else if (strcmp(argv[i]+2, "empty")) {
                empty = true;
            }
            else if (strcmp(argv[i]+2, "filled")) {
                filled = true;
            }
        }
        else {
            if (atoi(argv[i]) > 0) {
                argInt = atoi(argv[i]);
            }
            else {
                argStr = argv[i];
            }
        }
    }

    if (help) {
        printHelp();
        return 0;
    }

    // Set play to true if generate and solve are false
    if (!generate && !solve)
        play = true;

    // Making sure incompatible arguments havent been given
    if (generate && solve || generate && play || solve && play) {
        std::cout << "Incompatible arguments sUpplied.\nEither generate, solve or play\n";
        return 1;
    }

    if (solve && empty || solve && filled) {
        std::cout << "This program only supports completely solving puzzles\nAll squares will be filled.\n";
    }

    if (empty && filled) {
        std::cout << "Either supply the number of empty squares or the number of squares to fill.\n";
        return 1;
    }

    if (file && !argStr) {
        std::cout << "No file name given.\n";
        return 1;
    }

    if ((empty && !argInt) || (filled && !argInt)) {
        std::cout << "Requires a numeric argument";
        return 1;
    }
    // Done parsing arguments

    if (generate) {
        if (filled) {
            argInt = 81 - argInt;
        }
        if (argInt > 64) {
            std::cout << "No unique sudoku puzzle exists with fewer than 17 given digits.\n";
        }
        Generator gen = (filled || empty) ? Generator(argInt) : Generator();
        if (file) {
            std::fstream fileStream;
            fileStream.open(argStr);
            gen.createBoard().printBoard(fileStream);
            return 0;
        }
        gen.createBoard().printBoard();
        return 0;
    }

    if (solve) {
        if (file) {
            Board b = file::getPuzzle(argStr);
            b.printSolution();
        }
        else {
            std::cout << "TODO: create a way to interactively input puzzle";
        }
        return 0;
    }

    if (play) {
        Board b = file ? file::getPuzzle(argStr) : ((empty || filled) ? Generator(argInt).createBoard() : Generator().createBoard());
        Game game(b);
        game.mainLoop();
        return 0;
    }
    
}