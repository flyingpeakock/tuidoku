#include "Game.h"
#include <string.h>
#include <string>
#include <fstream>
#include <chrono>
#include <random>
#include <sstream>

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

std::string getRandomXMLPuzzle(const char *fileName) {
    std::ifstream file;
    std::string line;
    std::vector<std::string> puzzles;
    file.open(fileName);
    while (getline(file, line)) {
        std::size_t found = line.find("<game data=");
        if (found == std::string::npos)
            continue;
        std::size_t begin = line.find("'");
        if (begin == std::string::npos)
            begin = line.find("\"");
        std::size_t end = line.rfind("'");
        if (end == std::string::npos)
            end = line.rfind("\"");
        puzzles.emplace_back(line.substr(begin+1, end - begin - 1));
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);
    file.close();
    return puzzles[rand() % puzzles.size()];
}

std::string getRandomSdmPuzzle(const char *fileName) {
    std::ifstream file;
    std::vector<std::string> puzzles;
    std::string line;
    file.open(fileName);
    while(getline(file, line)) {
        puzzles.push_back(line);
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);
    file.close();
    return puzzles[rand() % puzzles.size()];
}

std::string getSSPuzzle(const char *fileName) {
    std::ifstream file;
    std::stringstream puzzle;
    std::string line;
    file.open(fileName);
    while (file.good()) {
        char c = file.get();
        if ('1' <= c || '9' >= c) {
            puzzle << c;
        }
        else if ('.' == c || 'X' == c) {
            puzzle << '0';
        }
    }
}

int main(int argc, char *argv[]) {
    Generator *gen = nullptr;
    for (auto i = 1; i < argc; i++) {
        if (atoi(argv[i]) > 0) {
            gen = new Generator(atoi(argv[i]));
        }
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printHelp();
            return 0;
        }
        if (strcmp(argv[i], "--opensudoku") == 0 && !gen) {
            gen = new Generator(getRandomXMLPuzzle(argv[i+1]).c_str());
        }
        if (strcmp(argv[i], "--sdm") == 0 && !gen) {
            gen = new Generator(getRandomSdmPuzzle(argv[i+1]).c_str());
        }
    }
    if (!gen)
        gen = new Generator(64);
    Board board(*gen);
    board.printBoard();
    /*Game game(board);
    game.mainLoop();*/
    delete gen;
}