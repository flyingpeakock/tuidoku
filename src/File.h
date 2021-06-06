#pragma once
#include "Board.h"

namespace file {
    Board getSDKPuzzle(const char *fileName);
    Board getRandomXMLPuzzle(const char *fileName);
    Board getRandomSDMPuzzle(const char *fileName);
    Board getSSPuzzle(const char *fileName);
    Board getPuzzle(const char *fileName);
}