#include "Sudoku.h"
#include <random>
#include <vector>

static bool backTrack(Sudoku::DancingLinkTable *table, Sudoku::DancingLink **current_solution, unsigned int depth, unsigned int *solution_count, bool randomize);

/**
 * @brief find the column with the fewest amount of rows
 * 
 * @return ColHeader 
 */
static Sudoku::DancingLinkColumn *smallestColumn(Sudoku::DancingLink *root, bool randomize);

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
        table->solution.clear();
        for (auto i = 0; i < depth; i++) {
            table->solution.push_back(current_solution[i]);
        }
        return true;
    }

    Sudoku::DancingLinkColumn *col = smallestColumn(&table->root, randomize);

    col->cover();
    for (Sudoku::DancingLink *row = col->down; row != col; row = row->down) {
        current_solution[depth] = row;
        Sudoku::cover_link(row);
        should_ret = backTrack(table, current_solution, depth+1, solution_count, randomize) && *solution_count > 1;
        Sudoku::uncover_link(row);
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
static Sudoku::DancingLinkColumn *smallestColumn(Sudoku::DancingLink *root, bool randomize) {
    Sudoku::DancingLinkColumn *current;
    Sudoku::DancingLinkColumn *ret;
    int min = 0xFFFF;
    if (!randomize) {
        for (current = (Sudoku::DancingLinkColumn *)root->right; (Sudoku::DancingLink *)current != root; current = (Sudoku::DancingLinkColumn*)current->right) {
            if (current->count < min) {
                min = current->count;
                ret = current;
                if (min == 0) return current;
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
