#include "Arguments.h"
#include <string.h>
#include <iostream>


void arguments::printHelp() {
    const char *helpText = "usage: tuidoku [OPTIONS]\n\n"
                           "Play, solve and generate sudoku puzzles in the terminal.\n\n"
                           "Optional args:\n"
                           "-h --help\t\tView this page\n"
                           "-p --play\t\tPlay sudoku. Default.\n"
                           "-g --generate\t\tGenerate a sudoku puzzle.\n"
                           "-s --solve\t\tSolve a sudoku puzzle.\n"
                           "-f --file\t\tRead or write to a file. Requires a file name.\n"
                           "-e --empty\t\tNumber of empty squares in board. Requires a number.\n\t\t\tCannot be used with -F.\n"
                           "-F --filled\t\tNumber of filled squares in board. Requires a number.\n\t\t\tCannot be used with -e.\n\n"
                           "Configuration is done by editing the file config.h\n"
                           "That file also contains all the keybinds.";
    std::cout << helpText << std::endl;
}

std::map<std::string, bool> arguments::parse(int argc, char *argv[]) {
    std::map<std::string, bool> args;
    for (auto i = 1; i < argc; i++) {

        if (argv[i][0] == '-' && argv[i][1] != '-') {
            // Single letter arguments, loop through every letter
            for (auto j = 1; j < strlen(argv[i]); j++) {
                char c = argv[i][j];
                switch(c) {
                    case 'h':
                    args["help"] = true;
                    break;
                    case 'p':
                    args["play"] = true;
                    break;
                    case 'g':
                    args["generate"] = true;
                    break;
                    case 's':
                    args["solve"] = true;
                    break;
                    case 'f':
                    args["file"] = true;
                    break;
                    case 'e':
                    args["empty"] = true;
                    break;
                    case 'F':
                    args["filled"] = true;
                    break;
                }
            }
        }

        else if (argv[i][0] == '-' && argv[i][1] == '-') {
            // Long form arguments
            if (strcmp(argv[i]+2, "help") == 0) {
                args["help"] = true;
            }
            else if (strcmp(argv[i]+2, "play") == 0) {
                args["play"] = true;
            }
            else if (strcmp(argv[i]+2, "generate") == 0) {
                args["generate"] = true;
            }
            else if (strcmp(argv[i]+2, "solve") == 0) {
                args["solve"] = true;
            }
            else if (strcmp(argv[i]+2, "file") == 0) {
                args["file"] = true;
            }
            else if (strcmp(argv[i]+2, "empty") == 0) {
                args["empty"] = true;
            }
            else if (strcmp(argv[i]+2, "filled") == 0) {
                args["filled"] = true;
            }
        }
    }
    if (!args["generate"] && !args["solve"]) {
        args["play"] = true;
    }
    return args;
}

std::string arguments::getFileName(int argc, char *argv[]) {
    for (auto i = 1; i < argc; i++) {
        if (argv[i][0] == '-')
            continue;
        
        if (atoi(argv[i]) == 0) {
            return argv[i];
        }
    }
    return "404";
}

int arguments::getInt(int argc, char *argv[]) {
    for (auto i = 1; i < argc; i++) {
        if (argv[i][0] == '-')
            continue;
        int ret = atoi(argv[i]);
        if (ret > 0 && ret >= 81)
            return ret;
    }
    return 0;
}

bool arguments::incompatible(std::map<std::string, bool> args) {
    if ((args["generate"] && args["solve"]) || (args["generate"] && args["play"]) || (args["solve"] && args["play"])) {
        std::cout << "Incompatible arguments supplied.\nEither generate, solve, or play.\n";
        return true;
    }

    if ((args["solve"] && args["empty"]) || (args["solve"] && args["filled"])) {
        std::cout << "This program only supports completely solving puzzles.\n";
        return true;
    }

    if (args["empty"] && args["filled"]) {
        std::cout << "Either supply the number of empty or the number of filled boxes. Not both.\n";
        return true;
    }
    return false;
}