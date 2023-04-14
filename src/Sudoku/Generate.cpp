#include "Sudoku.h"
#include <vector>
#include <cstring>

Sudoku::DancingLinkTable Sudoku::generate() {
    // TODO: Figure out which to remove first, maybe by looking at which column has the most rows in each step during solve and picking one of those
    bool solve_result = true;
    size_t index = 0;
    size_t current_index = 0;
    std::vector<DancingLink *> solution;
    DancingLinkTable ret(true);
    DancingLink *col;
    DancingLink *row;

    solve(&ret, true); // fills the solution array, all columns are uncovered
    for (DancingLink **current = ret.solution; *current; current++) {
        solution.push_back(*current);
    }


    // Cover the solution
    for (auto &link : solution) {
        link->colHeader->cover();
        for (col = link->right; col != link; col = col->right) {
            col->colHeader->cover();
        }
    }

    while (solve_result) { // while puzzle is unique
        DancingLink *link = solution[index];
        for (col = link->left; col != link; col = col->left) {
            col->colHeader->uncover();
        }
        link->colHeader->uncover();
    
        solve_result = solve(&ret, false);
        if (solve_result) { // was ok to remove, leave uncovered and add to current
            ret.current[current_index++] = solution[index];
        }
        else { // not ok, cover again
            link->colHeader->cover();
            for (col = link->right; col != link; col = col->right) {
                col->colHeader->cover();
            }
        }
        index++;
    }


    return ret;
}
