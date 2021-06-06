#include "File.h"
#include "Generator.h"
#include <fstream>
#include <chrono>
#include <random>
#include <sstream>

Board file::getPuzzle(const char *fileName) {
    std::ifstream file;
    file.open(fileName);

    std::string fileStr(fileName);

    std::size_t found = fileStr.find(".sdm");
    if (found != std::string::npos) {
        return getRandomSDMPuzzle(file);
    }

    found = fileStr.find(".sdk");
    if (found != std::string::npos) {
        return getSDKPuzzle(file);
    }

    found = fileStr.find(".ss");
    if (found != std::string::npos) {
        return getSSPuzzle(file);
    }

    found = fileStr.find(".opensudoku");
    if (found != std::string::npos) {
        return getRandomXMLPuzzle(file);
    }


    // file does not end with regular file endings
    // checking for xml tags
    std::string line;
    while (file.good()) {
        char c = file.get();
        if (c == '<') {
            // assuming xml since it has <
            file.close();
            return getRandomXMLPuzzle(file);
        }
    }
    file.close();
    return getSDKPuzzle(file); // Attempting with this function
}

Board file::getSDKPuzzle(std::istream &file) {
    std::stringstream stringStream;
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
    return Generator{stringStream.str().c_str()}.createBoard();
}

Board file::getRandomXMLPuzzle(std::istream &file) {
    std::string line;
    std::vector<std::string> puzzles;
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
    return Generator{puzzles[rand() % puzzles.size()].c_str()}.createBoard();
}

Board file::getRandomSDMPuzzle(std::istream &file) {
    std::vector<std::string> puzzles;
    std::string line;
    while(getline(file, line)) {
        puzzles.push_back(line);
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand(seed);
    return Generator{puzzles[rand() % puzzles.size()].c_str()}.createBoard();
}

Board file::getSSPuzzle(std::istream &file) {
    std::stringstream puzzle;
    std::string line;
    while (file.good()) {
        char c = file.get();
        if ('1' <= c || '9' >= c) {
            puzzle << c;
        }
        else if ('.' == c || 'X' == c || 'x' == c || '0' == c) {
            puzzle << '0';
        }
    }
    return Generator{puzzle.str().c_str()}.createBoard();
}