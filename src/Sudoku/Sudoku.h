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

    /**
     * @brief Generate a sudoku puzzle with a unique solution
     * 
     * @return DancingLinkTable 
     */
    DancingLinkTable generate();

    /**
     * @brief Get the Row index of DancingLink
     * 
     * @param link 
     * @return int 0 - (eSize - 1)
     */
    inline int getRowFromLink(DancingLink *link) {
        return link->count / eBoardSize;
    }

    /**
     * @brief Get the Col index of DancingLink
     * 
     * @param link 
     * @return int 0 - (eSize - 1)
     */
    inline int getColFromLink(DancingLink *link) {
        return (link->count % eBoardSize) / eSize;
    }

    /**
     * @brief Get the num value of DancingLink
     * 
     * @param link 
     * @return int 1 - eSize
     */
    inline int getNumFromLink(DancingLink *link) {
        return (link->count % eSize) + 1;
    }

    /**
     * @brief Covers all the columns in a row, except for the column of link
     * 
     * @param link 
     */
    void cover_link(DancingLink *link);

    /**
     * @brief Uncovers all the columns in a row, except for the column of link
     * 
     * @param link 
     */
    void uncover_link(DancingLink *link);
};