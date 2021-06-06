#pragma once
#include "Board.h"

namespace file {
    Board getSDKPuzzle(std::istream &file);
    Board getRandomXMLPuzzle(std::istream &file);
    Board getRandomSDMPuzzle(std::istream &file);
    Board getSSPuzzle(std::istream &file);
    Board getPuzzle(const char *fileName);
}