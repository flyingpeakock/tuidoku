#ifndef DANCING_LINK_H
#define DANCING_LINK_H

#include <compare>               // for operator<, __synth3way_t
#include <vector>                // for vector

#include "DancingLinkObjects.h"  // for DancingLinkContainer, DancingLink

namespace sudoku {

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
    inline DancingLinkContainer::const_iterator containsLinkEquivalent(int row, int col, DancingLinkContainer::const_iterator start, DancingLinkContainer::const_iterator end) {
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

    inline DancingLinkContainer::const_iterator containsLinkEquivalent(DancingLink *link, DancingLinkContainer::const_iterator start, DancingLinkContainer::const_iterator end) {
        return containsLinkEquivalent(getRowFromLink(link), getColFromLink(link), start, end);
    }

    inline DancingLinkContainer::const_iterator containsLinkEqual(int row, int col, int num, DancingLinkContainer::const_iterator start, DancingLinkContainer::const_iterator end) {
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

    inline DancingLinkContainer::const_iterator containsLinkEqual(DancingLink *link, DancingLinkContainer::const_iterator start, DancingLinkContainer::const_iterator end) {
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

#endif // DANCING_LINK_H
