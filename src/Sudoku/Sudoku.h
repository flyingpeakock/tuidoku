#ifndef SUDOKU_H
#define SUDOKU_H

#include "Constants.h"
#include "DancingLink.h"
#include "SudokuPuzzle.h"
#include "Logic.h"
#include <random>
#include <thread>

namespace Sudoku {

    extern std::atomic<bool> kill_threads;

    /**
     * @brief solves a sudoku puzzle
     * 
     * @param table .. puzzle represented as a constraint table
     *                 the table should be fully created
     * @param randomize .. randomize for creating puzzles 
     * @return true .. if a unique solution
     * @return false .. if none or multiple solutions
     */
    bool solve(DancingLinkTable &table, bool randomize);

    /**
     * @brief Generate a sudoku puzzle with a unique solution
     * 
     * @param diff difficulty of board to generate
     * @return DancingLinkTable 
     */
    DancingLinkTable generate(difficulty diff);

    DancingLinkTable generate(std::string string);

    DancingLinkTable generate();


    /**
    * @brief Removed a link from the SudokuPuzzle type
    *
    * @param puzzle puzzle to remove from
    * @param row 0 - 8
    * @param col 0 - 8
    */
    void removeFromPuzzle(SudokuPuzzle *puzzle, int row, int col);

    bool canSee(DancingLink *link_l, DancingLink *link_r);

    /**
    * @brief find the column with the fewest amount of rows
    * 
    * @return ColHeader 
    */
    static DancingLinkColumn *smallestColumn(Sudoku::DancingLink *root, bool randomize) {
        Sudoku::DancingLinkColumn *current;
        Sudoku::DancingLinkColumn *ret;
        int min = 0xFFFF;
        if (!randomize) {
            for (current = (Sudoku::DancingLinkColumn *)root->right; (Sudoku::DancingLink *)current != root; current = (Sudoku::DancingLinkColumn*)current->right) {
                if (current->count < min) {
                    min = current->count;
                    ret = current;
                }
            }
        }
        else {
            std::vector<Sudoku::DancingLinkColumn *> potentialColumns;
            for (current = (Sudoku::DancingLinkColumn *)root->right; (Sudoku::DancingLink *)current != root; current = (Sudoku::DancingLinkColumn *)current->right) {
                if (current->count < min) {
                    potentialColumns.clear();
                    potentialColumns.push_back(current);
                    min = current->count;
                    if (min == 0) return current;
                }
                else if (current->count == min) {
                    potentialColumns.push_back(current);
                }
            }
            if (potentialColumns.size() == 1) return potentialColumns[0];
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, potentialColumns.size() - 1);
            ret = potentialColumns[distrib(gen)];
        }
        return ret;
    }
};

#endif // SUDOKU_H
