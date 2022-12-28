#include "File.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include <sstream>
#include "Sudoku/Sudoku.h"

std::vector<Sudoku::puzzle> file::getPuzzle(const char *fileName) {
    std::ifstream file;
    file.open(fileName);

    std::string fileStr(fileName);

    std::size_t found = fileStr.find(".sdm");
    if (found != std::string::npos) {
        return getSDMPuzzle(file);
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
        return getXMLPuzzle(file);
    }

    // file does not end with regular file endings
    // checking for xml tags
    std::string line;
    while (file.good()) {
        char c = file.get();
        if (c == '<') {
            // assuming xml since it has <
            return getXMLPuzzle(file);
        }
    }
    file.clear();
    
    return getSDKPuzzle(file); // Attempting with this function
}

std::vector<Sudoku::puzzle> file::getStringPuzzle(const char *puzzleString) {
    std::stringstream puzzle(puzzleString);

    if (puzzle.str().find('<') != std::string::npos) {
        return getXMLPuzzle(puzzle);
    }
    return getSDKPuzzle(puzzle);
}

/**
 * @brief creates boards from an SDK puzzle file
 * 
 * @param file
 * @return std::vector<Sudoku::puzzle> 
 */
std::vector<Sudoku::puzzle> file::getSDKPuzzle(std::istream &file) {
    file.seekg(std::ios_base::beg);
    std::stringstream stringStream;
    std::string line;
    while (getline(file, line)) {
        if (line.front() == '#')
            continue;
        for (char c : line) {
            if ('1' <= c && '9' >= c) {
                stringStream << c;
            }
            else if ('.' == c || 'X' == c || '0' == c || 'x' == c) {
                stringStream << '0';
            }
        }
    }
    //return std::vector<Sudoku::puzzle>{Generator{stringStream.str().c_str()}.createSudoku::puzzle()};
    return std::vector<Sudoku::puzzle>{Sudoku::fromString(stringStream.str())};
}

std::vector<Sudoku::puzzle> file::getXMLPuzzle(std::istream &file) {
    file.seekg(std::ios_base::beg);
    std::string line;
    std::vector<Sudoku::puzzle> puzzles;
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
        //Sudoku::puzzle b = Generator{(line.substr(begin+1, end- begin - 1)).c_str()}.createSudoku::puzzle();
        Sudoku::puzzle b = Sudoku::fromString(line.substr(begin+1, end - begin - 1).c_str());
        puzzles.push_back(b);
    }
    return puzzles;
}

/**
 * @brief Parses sdm files for puzzles
 * 
 * @param file containing the puzzles in sdm format
 * @return std::vector<Sudoku::puzzle> containing puzzles in the file
 */
std::vector<Sudoku::puzzle> file::getSDMPuzzle(std::istream &file) {
    file.seekg(std::ios_base::beg);
    std::vector<Sudoku::puzzle> puzzles;
    std::string line;
    while(getline(file, line)) {
        //Generator g = Generator(line.c_str());
        puzzles.emplace_back(Sudoku::fromString(line));
    }
    return puzzles;
}

/**
 * @brief parses ss files for puzzles
 * 
 * @param file containing the puzzle
 * @return std::vector<Sudoku::puzzle> containing the puzzle
 */
std::vector<Sudoku::puzzle> file::getSSPuzzle(std::istream &file) {
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
    //return std::vector<Sudoku::puzzle>{Generator{puzzle.str().c_str()}.createSudoku::puzzle()};
    return std::vector<Sudoku::puzzle>{Sudoku::fromString(puzzle.str())};
}

std::vector<Sudoku::puzzle> file::getTuidokuPuzzle(std::istream &file) {
    file.seekg(std::ios_base::beg);
    std::ostringstream sstr;
    std::string rawPuzzle;
    sstr << file.rdbuf();
    rawPuzzle = sstr.str();
    
    std::ostringstream puzzleStream;
    for (size_t i = 0; i < rawPuzzle.length(); i++) {
        char c = rawPuzzle[i];
        if ('1' <= c && '9' >= c) {
            puzzleStream << c;
        }
        else if (c == ' ' && rawPuzzle[i+1] == ' ' && rawPuzzle[i+2] == ' ') {
            puzzleStream << '0';
            i += 2;
        }
        if (puzzleStream.tellp() >= 81) {
            break;
        }
    }
    //return std::vector<Sudoku::puzzle>{Generator{puzzleStream.str().c_str()}.createSudoku::puzzle()};
    return std::vector<Sudoku::puzzle>{Sudoku::fromString(puzzleStream.str())};
}
