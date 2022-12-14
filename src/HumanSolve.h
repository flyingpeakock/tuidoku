#pragma once
#include "Board.h"
#include "config.h"

bool solveHuman(Board *board);
bool findNakedSingles(Board *board);
bool findHiddenSingles(Board *board);
bool findPointingBox(Board *board);
bool findNakedDoubles(Board *board);
