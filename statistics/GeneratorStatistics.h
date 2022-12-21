#pragma once
#include "../src/Board.h"

struct SingleStats {
    int emptyGrids;
    int difficulty;
    int movesToSolve;
};

SingleStats gradeBoard(Board &board);

void accumulateBoardGrades(int count);