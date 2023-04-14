#include "Sudoku.h"
#include <vector>
#include <cstring>

Sudoku::DancingLinkTable Sudoku::generate() {
    // TODO: Figure out which to remove first, maybe by looking at which column has the most rows in each step during solve and picking one of those
    bool solve_result = true;
    size_t current_index = 0;
    std::vector<DancingLink *> solution;
    DancingLinkTable ret(true);
    DancingLink *col;

    solve(&ret, true); // fills the solution array, all columns are uncovered
    solution = ret.solution;
    ret.current = ret.solution;

    for (auto i = 0; i < eBoardSize; i++) {
        repairLinks(&ret);

        // Uncovering the latest covered
        for (col = ret.current.back()->left; col != ret.current.back(); col = col->left) {
            col->colHeader->uncover();
        }
        ret.current.back()->colHeader->uncover();
        
        if (!solve(&ret, false)) {
            // move to front so we don't try it again
            ret.current.insert(ret.current.begin(), ret.current.back());
        }

        // remove either duplicate or unnessary
        ret.current.pop_back();
    }
    return ret;
}
