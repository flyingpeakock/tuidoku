#pragma once
#include "Constants.h"
#include <vector>
#include <array>

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
        std::array<DancingLink, eConstraints> colHeaders;
        std::array<DancingLink, eBufferSize> buffer;
        std::vector<DancingLink *>current;
        std::vector<DancingLink *>solution;
    };

}