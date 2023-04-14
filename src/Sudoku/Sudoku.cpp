#include "Sudoku.h"

void Sudoku::cover_link(DancingLink *link) {
    for (auto col = link->right; col != link; col = col->right) {
        col->colHeader->cover();
    }
}

void Sudoku::uncover_link(DancingLink *link) {
    for (auto col = link->left; col != link; col = col->left) {
        col->colHeader->uncover();
    }
}
