#pragma once
#include "Board.h"
#include "config.h"

bool solveHuman(Board *board);
bool findNakedSingles(Board *board, const std::uint16_t num);
bool findHiddenSingles(Board *board, const std::uint16_t num);
bool findPointingBox(Board *board, char i_start, char j_start);
bool findLockedCandidates(Board *board);
bool findHidden(Board *board, const std::uint16_t num);
bool findNaked(Board *board, const std::uint16_t num);
bool findBug(Board *board);
bool findXwing(Board *board, const std::uint16_t num);
bool findUniqueRectangle(Board *board, const std::uint16_t num);
