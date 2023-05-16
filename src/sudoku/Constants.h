#ifndef SUDOKU_CONSTANTS_H
#define SUDOKU_CONSTANTS_H

#include "DancingLinkObjects.h"

namespace sudoku {
    /**
     * @brief enums of board difficulties
     * 
     */
    enum difficulty {
        eBeginner,
        eEasy,
        eMedium,
        eHard,
        eExpert,
        ePro,
        eDifficulties,
        eHighestDifficulty = ePro,
        eAny
    };

    namespace logic {
        /** enum containing the type of move*/
        enum moveType {
            eLogicPencil, // Found a pencil mark to remove
            eLogicInsert, // Found a cell that can be inserted into
            eLogicErrorPencil, // Found a pencilMark that was wrongly removed
            eLogicErrorInsert, // Found an inserted number that is wrong
            eLogicErrorPencilMissing, // Found a pencil mark that isn't visible
            eMoveNotFound
        };
        
        /**
         * @brief move containig vector of truths and falses, move type and difficulty
         * 
         */
        struct LogicalMove {
            LogicalMove() : type(eMoveNotFound) {}
            moveType type;
            difficulty diff;
            /**
             * @brief Vector contaiing truths found using logic
             * 
             * These are the links to be highlighted when searching for hints
             * Also contains the links to insert if eLogicInsert
             * empty when eLogicError
             */
            DancingLinkContainer truths;

            /**
             * @brief Vector containing falses found using logic
             * 
             * These are the links to removed from pencil marks if eLogicInsert
             * Or the links that are wrong if eLogicErorr
             */
            DancingLinkContainer falses;
        };
    }

} // namespace sudoku

#endif // SUDOKU_CONSTANTS_H
