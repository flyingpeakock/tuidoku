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

        /**
         * @brief Generates an array of vectors with the same number of link per column
         *
         * @param root root of the constraint table
         * @return std::array<std::vector<Sudoku::DancingLink *>, Sudoku::eSize> 
         */
        std::array<std::vector<Sudoku::DancingLink *>, Sudoku::eSize> getSortedConstraintColumns(DancingLink *root);

        // Mistakes
        bool foundIllogicalInput(const SudokuPuzzle &puzzle, std::vector<Move> &moves);
        bool foundIllogicalPencil(const SudokuPuzzle &puzzle, std::vector<Move> &moves);
        bool foundWrongRemovedPencil(const SudokuPuzzle &puzzle, std::vector<Move> &moves);
        bool foundMissingPencilMark(const SudokuPuzzle &puzzle, std::vector<Move> &moves);
        bool foundWrongInput(const SudokuPuzzle &puzzle, std::vector<Move> &moves);

        // Beginner
        bool foundSingle(const std::vector<DancingLink *> &columns, std::vector<Move> &moves);

        // Basic moves
        /**
         * @brief Looks for locked candidates in the board
         * 
         * @param columns columns that are potential truths
         * @param moves vector to add to if found
         * @return true if a move is found
         * @return false if a move is not found
         */
        bool foundLockedCandidates(const std::vector<DancingLink *> &columns, std::vector<Move> &moves);

        /**
         * @brief Searches for any mistakes in the board
         * 
         * @param puzzle puzzle to search through
         * @param move move which gets set if a mistake is found
         * @return true if a mistake is found
         * @return false if no mistkes are found
         */
        std::vector<Move> foundMistake(const SudokuPuzzle &puzzle);

        /**
         * @brief Get the Next Move found in puzzle
         * 
         * @param puzzle SubokuPuzzle object
         * @param foundMove gets set true if a move if found, otherwise sets to false
         * @return Move move object
         */
        std::vector<Move> getNextMove(const SudokuPuzzle &puzzle, bool ignoreMistakes);
    }
}
