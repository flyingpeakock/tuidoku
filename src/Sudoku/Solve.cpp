#include <memory>                // for shared_ptr, __shared_ptr_access

#include "Sudoku.h"              // for smallestColumn, solve
#include "DancingLink.h"         // for cover_link, uncover_link
#include "DancingLinkObjects.h"  // for DancingLinkColumn, DancingLinkTable

static bool backTrack(Sudoku::DancingLinkTable &table, Sudoku::DancingLink **current_solution, unsigned int depth, unsigned int *solution_count, bool randomize);

bool Sudoku::solve(DancingLinkTable &table, bool randomize) {
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
static bool backTrack(Sudoku::DancingLinkTable &table, Sudoku::DancingLink **current_solution, unsigned int depth, unsigned int *solution_count, bool randomize) {
    bool should_ret = false;
    if (table.root->right == table.root.get()) {
        (*solution_count)++;
        
        // This solution becomes the solution in the table
        table.solution.clear();
        for (auto i = 0; i < depth; i++) {
            table.solution.push_back(current_solution[i]);
        }
        return true;
    }

    Sudoku::DancingLinkColumn *col = smallestColumn(table.root.get(), randomize);

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
