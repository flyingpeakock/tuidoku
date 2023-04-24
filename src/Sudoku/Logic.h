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
        struct Move {
            moveType type;
            difficulty diff;
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

        // Mistakes
        bool foundIllogicalInput(const SudokuPuzzle &puzzle, Move &move);
        bool foundIllogicalPencil(const SudokuPuzzle &puzzle, Move &move);
        bool foundWrongRemovedPencil(const SudokuPuzzle &puzzle, Move &move);
        bool foundMissingPencilMark(const SudokuPuzzle &puzzle, Move &move);
        bool foundWrongInput(const SudokuPuzzle &puzzle, Move &move);

        // Beginner
        bool foundSingle(DancingLink *root, Move &move);

        /**
         * @brief Searches for any mistakes in the board
         * 
         * @param puzzle puzzle to search through
         * @param move move which gets set if a mistake is found
         * @return true if a mistake is found
         * @return false if no mistkes are found
         */
        bool foundMistake(const SudokuPuzzle &puzzle, Move &move);

        /**
         * @brief Get the Next Move found in puzzle
         * 
         * @param puzzle SubokuPuzzle object
         * @param foundMove gets set true if a move if found, otherwise sets to false
         * @return Move move object
         */
        Move getNextMove(const SudokuPuzzle &puzzle, bool &foundMove);
    }
}