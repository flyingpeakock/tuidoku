#pragma once
#include "Board.h"
#include <vector>

namespace file {
    std::vector<Board> getSDKPuzzle(std::istream &file);
    std::vector<Board> getXMLPuzzle(std::istream &file);
    std::vector<Board> getSDMPuzzle(std::istream &file);
    std::vector<Board> getSSPuzzle(std::istream &file);
    std::vector<Board> getPuzzle(const char *fileName);
    std::vector<Board> getStringPuzzle(const char *puzzleString);
    std::vector<Board> getTuidokuPuzzle(std::istream &file);
}