#include "File.h"
#include <iostream>
#include "Generator.h"
#include <fstream>
#include <chrono>
#include <random>
#include <sstream>
#include "config.h"

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
            return getRandomXMLPuzzle(file);
        }
    }
    file.clear();
    
    file.seekg(std::ios_base::beg);
    while (file.good()) {
        char c = file.get();
        if (c == TOPROW[1]) {
            return getTuidokuPuzzle(file);
        }
    }
    file.clear();
    return getSDKPuzzle(file); // Attempting with this function
}

Board file::getStringPuzzle(const char *puzzleString) {
    std::stringstream puzzle(puzzleString);

    if (puzzle.str().find(TOPROW) != std::string::npos) {
        return getTuidokuPuzzle(puzzle);
    }
    puzzle.seekg(std::ios_base::beg);
    if (puzzle.str().find('<') != std::string::npos) {
        return getRandomXMLPuzzle(puzzle);
    }
    return getSDKPuzzle(puzzle);
}

Board file::getSDKPuzzle(std::istream &file) {
    file.seekg(std::ios_base::beg);
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
    file.seekg(std::ios_base::beg);
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
    file.seekg(std::ios_base::beg);
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
    file.seekg(std::ios_base::beg);
    std::stringstream puzzle;
    while (file.good()) {
        char c = file.get();
        if ('1' <= c && '9' >= c) {
            puzzle << c;
        }
        else if ('.' == c || 'X' == c || 'x' == c || '0' == c) {
            puzzle << '0';
        }
    }
    return Generator{puzzle.str().c_str()}.createBoard();
}

Board file::getTuidokuPuzzle(std::istream &file) {
    file.seekg(std::ios_base::beg);
    std::ostringstream sstr;
    std::string rawPuzzle;
    sstr << file.rdbuf();
    rawPuzzle = sstr.str();
    
    std::ostringstream puzzleStream;
    for (auto i = 0; i < rawPuzzle.length(); i++) {
        char c = rawPuzzle[i];
        if ('1' <= c && '9' >= c) {
            puzzleStream << c;
        }
        else if (c == ' ' && rawPuzzle[i+1] == ' ' && rawPuzzle[i+2] == ' ') {
            puzzleStream << '0';
        }
    }
    return Generator{puzzleStream.str().c_str()}.createBoard();
}
