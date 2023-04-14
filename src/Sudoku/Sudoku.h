#pragma once
#include "Constants.h"
#include "DancingLink.h"

namespace Sudoku {
    /**
     * @brief solves a sudoku puzzle
     * 
     * @param table .. puzzle represented as a constraint table
     *                 the table should be fully created
     * @param randomize .. randomize for creating puzzles 
     * @return true .. if a unique solution
     * @return false .. if none or multiple solutions
     */
    bool solve(DancingLinkTable *table, bool randomize);

    DancingLinkTable generate();

    int getRowFromLink(DancingLink *link);
    int getColFromLink(DancingLink *link);
    int getNumFromLink(DancingLink *link);
};