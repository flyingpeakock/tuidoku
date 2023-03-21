#pragma once
#include "../src/Play.h"

struct SingleStats {
    int emptyGrids;
    int difficulty;
    int movesToSolve;
};

SingleStats gradeBoard(Play &board);

void accumulateBoardGrades(int count);