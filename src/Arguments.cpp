#include "Arguments.h"
#include <string.h>
#include <iostream>

arguments::arguments(int argc, char *argv[]) {
    argInt = 0;
    parse(argc, argv);
    if (args["file"]) {
        argStr = getFileName(argc, argv);
    }
    if (args["filled"] || args["empty"]) {
        argInt = getInt(argc, argv);
        if (args["filled"] && argInt != 0) {
            argInt = 81 - argInt;
        }
    }
}

bool arguments::printHelp() {
    if (!args["help"]) {
        return false;
    }
    const char *helpText = "usage: tuidoku [OPTIONS][FILE]\n\n"
                           "Play, solve and generate sudoku puzzles in the terminal.\n\n"
                           "Optional args:\n"
                           "-h --help\t\tView this page\n"
                           "-p --play\t\tPlay sudoku. Default.\n"
                           "-b --big\t\tUse a bigger board. Only has an effect with play.\n"
                           "-g --generate\t\tGenerate a sudoku puzzle.\n"
                           "-s --solve\t\tSolve a sudoku puzzle.\n"
                           "-f --file\t\tRead or write to a file. Requires a file name.\n"
                           "-e --empty\t\tNumber of empty squares in board. Requires a number.\n\t\t\tCannot be used with -F.\n"
                           "-F --filled\t\tNumber of filled squares in board. Requires a number.\n\t\t\tCannot be used with -e.\n\n"
                           "Configuration is done by editing the file config.h\n"
                           "That file also contains all the keybinds.\n\n"
                           "Play\n"
                           "The default mode is to play a randomly generated puzzle. To play a\n"
                           "puzzle from a file pass '-f' or '--file' and the file name to the program.\n"
                           "Move around the board and switch modes with the keys defined in config.h\n"
                           "The most recently pressed number is highlighted on the board\n"
                           "Pencil Marks\n"
                           "This game only supports 3 pencil marks per cell due to space constraints.\n"
                           "Pencil marks are automaticcally removed after they become invalidated,\n"
                           "they are also restored if a filled in square become un-filled\n\n"
                           "Generate\n"
                           "To generate a puzzle pass the argument '-g' or '--generate'.\n"
                           "To save the generated puzzle to a file pass '-f' or '--file'\n"
                           "along with the file name to save to. Pass '-e' or '--empty' to"
                           "specify the number of empty squares OR '-F' or '--filled' to\n"
                           "specify the number of squares to fill in. Be aware that generating\n"
                           "difficult puzzles may take a very long time.\n\n"
                           "Solve\n"
                           "To solve a puzzle pass '-s' or '--solve' to input a puzzle to generate.\n"
                           "The puzzle will automatically be solved once a unique solution is found.\n"
                           "Take care when inputting the puzzle as a puzzle without any solutions\n"
                           "will cause an infinite loop.\n"
                           "To solve a puzzle from a file pass the argument '-f' or '--file'\n"
                           "along with the file name. This program is able to parse SDK, opensudoku,\n"
                           "SDM, SS and puzzles generated by this program. However it will attempt to\n"
                           "parse other files as well.\n";
    std::cout << helpText << std::endl;
    return true;
}

std::map<std::string, bool> arguments::parse(int argc, char *argv[]) {
    //std::map<std::string, bool> args;
    for (auto i = 1; i < argc; i++) {

        if (argv[i][0] == '-' && argv[i][1] != '-') {
            // Single letter arguments, loop through every letter
            for (size_t j = 1; j < strlen(argv[i]); j++) {
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
                    case 'b':
                    args["big"] = true;
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
            else if (strcmp(argv[i]+2, "big") == 0) {
                args["big"] = true;
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
        if (ret > 0 && ret <= 81)
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

bool arguments::shouldExit() {
    if (args["file"] && argStr == "404") {
        std::cout << "No filed name supplied.\n";
        return true;
    }
    if ((args["empty"] || args["filled"]) && argInt == 0) {
        std::cout << "No number supplied.\n";
        return true;
    }
    return incompatible(args);
}

feature arguments::getFeature() {
    if (args["solve"]) {
        return feature::Solve;
    }
    if (args["generate"]) {
        return feature::Generate;
    }
    return feature::Play;
}

int arguments::getArgInt() {
    return argInt;
}

std::string arguments::getFileName() {
    return argStr;
}

bool arguments::fileArgSet() {
    return args["file"];
}

bool arguments::bigBoard() {
    return args["big"];
}