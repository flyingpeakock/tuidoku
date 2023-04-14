#pragma once
#include "Constants.h"

namespace Sudoku {
    void calculateConstraintColumns(int columns[4], int row, int col, int num);

    /**
     * @brief Dancing link which is a quadruply linked list
     * 
     */
    struct DancingLink {
        DancingLink *up;
        DancingLink *down;
        DancingLink *left;
        DancingLink *right;
        DancingLink *colHeader;
        int count; // is the column in columns, otherwise is the position and value in the board

        void cover();
        void uncover();
    };

    /**
     * @brief contains all the links needed for a sudoku constraint table
     * 
     */
    struct DancingLinkTable {
        DancingLinkTable(bool should_randomize);
        DancingLink root;
        DancingLink colHeaders[eConstraints];
        DancingLink buffer[eBufferSize];
        DancingLink *current[eBoardSize + 1] = {nullptr, };
        DancingLink *solution[eBoardSize + 1] = {nullptr, };
    };

    void repairLinks(DancingLinkTable *table);
    void coverRow(DancingLink *row);
    void uncoverRow(DancingLink *row);
}