#pragma once

#include "Sudoku.h"
#include <array>

namespace Sudoku {
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

        // Mistakes
        bool foundIllogicalInput(const SudokuPuzzle &puzzle, LogicalMove &move);
        bool foundIllogicalPencil(const SudokuPuzzle &puzzle, LogicalMove &move);
        bool foundWrongRemovedPencil(const SudokuPuzzle &puzzle, LogicalMove &move);
        bool foundMissingPencilMark(const SudokuPuzzle &puzzle, LogicalMove &move);
        bool foundWrongInput(const SudokuPuzzle &puzzle, LogicalMove &move);

        /**
         * @brief Looks for singles in the puzzle
         *        i.e last candidate in a row, box, col or cell
         * 
         * @param columns columns to search through
         * @param moves moves vector to add to
         * @return true if found something
         * @return false false if no moves found
         */
        bool foundSingle(const DancingLink* column, LogicalMove &move);

        /**
         * @brief Looks for locked candidates in the board
         * 
         * @param columns columns that are potential truths
         * @param moves vector to add to if found
         * @return true if a move is found
         * @return false if a move is not found
         */
        bool foundLockedCandidates(const DancingLink *column, const DancingLinkContainer &candidates, const DancingLinkContainer &links, LogicalMove &move);

        /**
         * @brief find hidden piars, naked pairs and x-wings
         * 
         * @param columns columns containing two candidates
         * @param moves moves vector to add to
         * @return true if any moves were found
         * @return false if no moves were found
         */
        bool xlogic(const DancingLink *column, LogicalMove &move);

        /**
         * @brief Searches for any mistakes in the board
         * 
         * @param puzzle puzzle to search through
         * @param move move which gets set if a mistake is found
         * @return true if a mistake is found
         * @return false if no mistkes are found
         */
        LogicalMove foundMistake(const SudokuPuzzle &puzzle);

        /**
         * @brief Get the Next Move found in puzzle
         * 
         * @param puzzle SubokuPuzzle object
         * @param foundMove gets set true if a move if found, otherwise sets to false
         * @return Move move object
         */
        LogicalMove getNextMove(const SudokuPuzzle &puzzle, bool ignoreMistakes);
    }
}
