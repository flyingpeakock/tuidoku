#pragma once
#include "Constants.h"
#include <vector>
#include <array>
#include <memory>

namespace Sudoku {
    /**
     * @brief Dancing link which is a quadruply linked list
     * 
     */
    struct DancingLinkColumn;
    struct DancingLink {
        DancingLink *up;
        DancingLink *down;
        DancingLink *left;
        DancingLink *right;
        DancingLinkColumn *colHeader;
        int count; // is the column in columns, otherwise is the position and value in the board
        bool isCoverd; // Only set in colHeaders
    };

    struct DancingLinkColumn : DancingLink {
        constraintColTypes constraintType;
        
        void cover();
        void uncover();
    };

    using DancingLinkContainer = std::vector<DancingLink *>;

    /**
     * @brief contains all the links needed for a sudoku constraint table
     * 
     */
    struct DancingLinkTable {
        DancingLinkTable(bool should_randomize);
        void generateLinks(bool should_randomize);
        std::shared_ptr<DancingLink> root;
        std::shared_ptr<DancingLinkColumn[]> colHeaders;
        std::shared_ptr<DancingLink[]> buffer;
        DancingLinkContainer current;
        DancingLinkContainer solution;
    };

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
     * @return int 0 - (eSize-1)
     */
    inline int getNumFromLink(DancingLink *link) {
        return (link->count % eSize);
    }

    /**
     * @brief check if a link is equal to the values being inserted
     * 
     * @param link to be checked
     * @param row 0 - 8
     * @param col 0 - 8
     * @param num 0 - 8 
     * @return true if is equal link
     * @return false if is not equal link
     */
    inline bool isLinkValues(DancingLink *link, int row, int col, int num) {
        int link_row, link_col, link_num;
        link_row = getRowFromLink(link);
        link_col = getColFromLink(link);
        link_num = getNumFromLink(link);

        return ((link_row == row) && (link_col == col) && link_num == num);
    }

    /**
     * @brief checks if a vector of links contains a link in the same position
     * 
     * @param row int
     * @param col int
     * @param start iterator begin
     * @param end iterator end
     * @return DancingLinkContainer::iterator equivalent if found, else end
     */
    inline DancingLinkContainer::iterator containsLinkEquivalent(int row, int col, DancingLinkContainer::iterator start, DancingLinkContainer::iterator end) {
        int cur_row, cur_col;
        for (auto i = start; i < end; i++) {
            cur_row = getRowFromLink(*i);
            cur_col = getColFromLink(*i);
            if ((row == cur_row) && (col == cur_col)) {
                return i;
            }
        }
        return end;
    }

    inline DancingLinkContainer::iterator containsLinkEqual(int row, int col, int num, DancingLinkContainer::iterator start, DancingLinkContainer::iterator end) {
        int cur_row, cur_col, cur_num;
        for (auto i = start; i < end; i++) {
            cur_row = getRowFromLink(*i);
            cur_col = getColFromLink(*i);
            cur_num = getNumFromLink(*i);
            if ((row == cur_row) && (col == cur_col) && (num == cur_num)) {
                return i;
            }
        }
        return end;
    }

    inline DancingLinkContainer::iterator containsLinkEqual(DancingLink *link, DancingLinkContainer::iterator start, DancingLinkContainer::iterator end) {
        for (auto i = start; i < end; i++) {
            if ((*i)->count == link->count) {
                return i;
            }
        }
        return end;
    }

    inline bool isUncovered(DancingLink * link) {
        if (link->colHeader->isCoverd) {
            return false;
        }

        return link->up->down == link;
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

    void uncover_link(DancingLink *link);

    void cover_row(DancingLink *link);

    void uncover_row(DancingLink *link);

    void calculateConstraintColumns(int columns[eConstraintTypes], int row, int col, int num);
}