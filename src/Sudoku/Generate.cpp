#include "Sudoku.h"
#include <vector>

void Sudoku::DancingLinkTables::generate(difficulty diff) {
    solve();
    DancingLink *orig_solution[eBoardSize];
    for (auto i = 0; i < eBoardSize; i++) {
        orig_solution[i] = solution[eBoardSize - (i+1)]->colHeader;
    }
    removeGivens(orig_solution);
}

void Sudoku::DancingLinkTables::removeGivens(DancingLink **solution) {
    std::vector<DancingLink *> remove_ok;
    for (auto i = 0; i < eBoardSize; i++) {
        for (auto ok : remove_ok) {
            ok->uncover();
        }
        DancingLink *header = solution[i];
        header->uncover();
        solve();
        if (solution_count == 1) {
            remove_ok.push_back(header);
        }
    }
}
