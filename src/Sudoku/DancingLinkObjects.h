#ifndef DANCING_LINK_OBJECTS_H
#define DANCING_LINK_OBJECTS_H

#include <vector>
#include <memory>

namespace Sudoku {

    enum constraintColTypes {
        eConstraintCell = 0,
        eConstraintRow,
        eConstraintCol,
        eConstraintBox,
        eConstraintTypes
    };

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
}

#endif