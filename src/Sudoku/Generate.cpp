#include "Sudoku.h"
#include <vector>
#include <cstring>

Sudoku::DancingLinkTable Sudoku::generate() {
    // TODO: Figure out which to remove first, maybe by looking at which column has the most rows in each step during solve and picking one of those
    bool solve_result = true;
    size_t current_index = 0;
    std::vector<DancingLink *> solution;
    DancingLinkTable ret(true);

    solve(&ret, true); // fills the solution array, all columns are uncovered
    solution = ret.solution;
    ret.current = ret.solution;

    //repairLinks(&ret);
    for (auto i = 0; i < eBoardSize; i++) {
        // Storing and removing last
        auto back = ret.current.back();
        ret.current.pop_back();

        // covering links left in current
        for (auto col = ret.current.begin(); col != ret.current.end(); col++) {
            (*col)->colHeader->cover();
            for (auto link = (*col)->right; link != (*col); link = link->right) {
                link->colHeader->cover();
            }
        }

        bool isUnique = solve(&ret, false);

        // uncovering links in opposite order
        for (auto col = ret.current.rbegin(); col != ret.current.rend(); col++) {
            for (auto link = (*col)->left; link != (*col); link = link->left) {
                link->colHeader->uncover();
            }
            (*col)->colHeader->uncover();
        }
        
        if (!isUnique) {
            // move to front so we don't try it again
            ret.current.insert(ret.current.begin(), back);
        }
    }
    return ret;
}
