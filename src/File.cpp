#include "File.h"
#include "Generator.h"
#include <fstream>
#include <chrono>
#include <random>
#include <sstream>

Board file::getPuzzle(const char *fileName) {
    std::string fileStr(fileName);

    std::size_t found = fileStr.find(".sdm");
    if (found != std::string::npos) {
        return getRandomSDMPuzzle(fileName);
    }

    found = fileStr.find(".sdk");
    if (found != std::string::npos) {
        return getSDKPuzzle(fileName);
    }

    found = fileStr.find(".ss");
    if (found != std::string::npos) {
        return getSSPuzzle(fileName);
    }

    found = fileStr.find(".opensudoku");
    if (found != std::string::npos) {
        return getRandomXMLPuzzle(fileName);
    }


    // file does not end with regular file endings
    // checking for xml tags
    std::ifstream file;
    file.open(fileName);
    std::string line;
    while (file.good()) {
        char c = file.get();
        if (c == '<') {
            // assuming xml since it has <
            file.close();
            return getRandomXMLPuzzle(fileName);
        }
    }
    file.close();
    return getSDKPuzzle(fileName); // Attempting with this function
}

Board file::getSDKPuzzle(const char *fileName) {
    std::stringstream stringStream;
    std::ifstream file;
    file.open(fileName);
    std::string line;
    while (getline(file, line)) {
        if (line.front() == '#')
            continue;
        for (auto c : line) {
            if ('1' <= c && '9' >= 'c') {
                stringStream << c;
            }
            else if ('.' == c || 'X' == c || '0' == c || 'x' == c) {
                stringStream << '0';
            }
        }
    }
    file.close();
    return Generator{stringStream.str().c_str()}.createBoard();
}

Board file::getRandomXMLPuzzle(const char *fileName) {
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
    return Generator{puzzles[rand() % puzzles.size()].c_str()}.createBoard();
}

Board file::getRandomSDMPuzzle(const char *fileName) {
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
    return Generator{puzzles[rand() % puzzles.size()].c_str()}.createBoard();
}

Board file::getSSPuzzle(const char *fileName) {
    std::ifstream file;
    std::stringstream puzzle;
    std::string line;
    file.open(fileName);
    while (file.good()) {
        char c = file.get();
        if ('1' <= c || '9' >= c) {
            puzzle << c;
        }
        else if ('.' == c || 'X' == c || 'x' == c || '0' == c) {
            puzzle << '0';
        }
    }
    file.close();
    return Generator{puzzle.str().c_str()}.createBoard();
}