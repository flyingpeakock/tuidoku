#include "Sudoku.h"

using namespace Sudoku;

/**
 * @brief find the column with the fewest amount of rows
 * 
 * @return ColHeader 
 */
static DancingLink *smallestColumn(DancingLink *root);

/**
 * @brief Solves a sudoku puzzle
 * 
 */
void Sudoku::DancingLinkTables::solve() {
    solution_idx = -1; // setting to -1 since backtrack increments before assignment
    solution_count = 0;
    // Solving the board
   backTrack();
}

/**
 * @brief Actual DLX algorithm
 * 
 * @param depth current depth, used for creating the board
 * @param solutions number of solutions found
 * @param root root of the constraint grid
 * @return true if a solution is found
 * @return false if no solution is found
 */
//bool backTrack(int depth, int &solutions, DancingLink *root, Sudoku::puzzle &grid, DancingLink **solutionSet, unsigned int *difficulty) {
bool Sudoku::DancingLinkTables::backTrack() {
    if (root.right == &root) {
        solution_count++;
        return true;
    }

    DancingLink *col = smallestColumn(&root);
    DancingLink *cur_col;

    col->cover();
    solution_idx++;
    for (DancingLink *row = col->down; row != col; row = row->down) {
        solution[solution_idx] = row;
        for (cur_col = row->right; cur_col != row; cur_col = cur_col->right) {
            cur_col->colHeader->cover();
        }
        if (backTrack() && solution_count > 1) {
            return true;
        }
        for (cur_col = row->left; cur_col != row; cur_col = cur_col->left) {
            cur_col->colHeader->uncover();
        }
    }
    solution_idx--;
    col->uncover();
    return false;
}

/**
 * @brief finds the smallest column
 * 
 * @param root 
 * @return DancingLink* to the smallest column
 */
static DancingLink *smallestColumn(DancingLink *root) {
    DancingLink *current;
    DancingLink *ret;
    int min = 0xFFFF;
    for (current = root->right; current != root; current = current->right) {
        if (current->count < min) {
            min = current->count;
            ret = current;
            if (min == 0)
                break;
        }
    }
    return ret;
}
