#include "Sudoku.h"

void Sudoku::DancingLinkTables::generate(difficulty diff) {
    if (solution_count != 1) {
        solve();
    }
    removeGivens();
}

//static Sudoku::puzzle removeGivens(Sudoku::puzzle filled) {
void Sudoku::DancingLinkTables::removeGivens() {
    std::vector<DancingLink *> remove_ok;
    for (auto i = (eSize * eSize) - 1; i >= 0; i--) {
        for (auto ok : remove_ok) {
            ok->uncover();
        }
        DancingLink *header = solution[i]->colHeader;
        header->uncover();
        solve();
        if (solution_count == 1) {
            remove_ok.push_back(header);
        }
    }
}
