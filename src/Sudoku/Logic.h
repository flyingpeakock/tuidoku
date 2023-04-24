#pragma once

#include "Sudoku.h"

namespace Sudoku {
    namespace logic {
        /** enum containing the type of move*/
        enum moveType {
            eLogicPencil, // Found a pencil mark to remove
            eLogicInsert, // Found a cell that can be inserted into
            eLogicErrorPencil, // Found a pencilMark that was wrongly removed
            eLogicErrorInsert, // Found an inserted number that is wrong
            eLogicErrorPencilMissing // Found a pencil mark that isn't visible
        };
        
        /**
         * @brief move containig vector of truths and falses, move type and difficulty
         * 
         */
        struct move {
            moveType type;
            difficulty difficulty;
            /**
             * @brief Vector contaiing truths found using logic
             * 
             * These are the links to be highlighted when searching for hints
             * Also contains the links to insert if eLogicInsert
             * empty when eLogicError
             */
            std::vector<DancingLink *> truths;

            /**
             * @brief Vector containing falses found using logic
             * 
             * These are the links to removed from pencil marks if eLogicInsert
             * Or the links that are wrong if eLogicErorr
             */
            std::vector<DancingLink *> falses;
        };
    }
}