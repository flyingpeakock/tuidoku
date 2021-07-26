#pragma once
#include "Board.h"
#include <vector>

namespace file {
    std::vector<SimpleBoard> getSDKPuzzle(std::istream &file);
    std::vector<SimpleBoard> getXMLPuzzle(std::istream &file);
    std::vector<SimpleBoard> getSDMPuzzle(std::istream &file);
    std::vector<SimpleBoard> getSSPuzzle(std::istream &file);
    std::vector<SimpleBoard> getPuzzle(const char *fileName);
    std::vector<SimpleBoard> getStringPuzzle(const char *puzzleString);
    std::vector<SimpleBoard> getTuidokuPuzzle(std::istream &file);
}