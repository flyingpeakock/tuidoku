#include "Sudoku.h"
#include <random>
#include <vector>

static bool backTrack(Sudoku::DancingLinkTable *table, Sudoku::DancingLink **current_solution, unsigned int depth, unsigned int *solution_count, bool randomize);

/**
 * @brief find the column with the fewest amount of rows
 * 
 * @return ColHeader 
 */
static Sudoku::DancingLink *smallestColumn(Sudoku::DancingLink *root, bool randomize);

bool Sudoku::solve(DancingLinkTable *table, bool randomize) {
    unsigned int solution_count = 0;
    DancingLink *solution[eBoardSize];
    backTrack(table, solution, 0, &solution_count, randomize);
    return solution_count == 1;
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
static bool backTrack(Sudoku::DancingLinkTable *table, Sudoku::DancingLink **current_solution, unsigned int depth, unsigned int *solution_count, bool randomize) {
    bool should_ret;
    if (table->root.right == &(table->root)) {
        (*solution_count)++;
        
        // This solution becomes the solution in the table
        for (auto i = 0; i < depth; i++) {
            table->solution[i] = current_solution[i];
        }
        return true;
    }

    Sudoku::DancingLink *col = smallestColumn(&table->root, randomize);
    Sudoku::DancingLink *cur_col;

    col->cover();
    for (Sudoku::DancingLink *row = col->down; row != col; row = row->down) {
        current_solution[depth] = row;
        for (cur_col = row->right; cur_col != row; cur_col = cur_col->right) {
            cur_col->colHeader->cover();
        }
        should_ret = backTrack(table, current_solution, depth+1, solution_count, randomize) && *solution_count > 1;
        for (cur_col = row->left; cur_col != row; cur_col = cur_col->left) {
            cur_col->colHeader->uncover();
        }
        if (should_ret) {
            col->uncover();
            return should_ret;
        }
    }
    col->uncover();
    return should_ret;
}

/**
 * @brief finds the smallest column
 * 
 * @param root 
 * @return DancingLink* to the smallest column
 */
static Sudoku::DancingLink *smallestColumn(Sudoku::DancingLink *root, bool randomize) {
    Sudoku::DancingLink *current;
    Sudoku::DancingLink *ret;
    int min = 0xFFFF;
    if (!randomize) {
        for (current = root->right; current != root; current = current->right) {
            if (current->count < min) {
                min = current->count;
                ret = current;
                if (min == 0) return current;
            }
        }
    }
    else {
        std::vector<Sudoku::DancingLink *> potentialColumns;
        for (current = root->right; current != root; current = current->right) {
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
