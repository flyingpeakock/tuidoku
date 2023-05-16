#include <random>  // for mt19937, random_device, uniform_int_distribution
#include <array>   // for array
#include <memory>  // for shared_ptr, __shared_ptr_access, make_shared

#include "DancingLink.h"

using namespace sudoku;

void DancingLinkColumn::cover() {
    DancingLink *i;
    DancingLink *j;
    isCoverd = true;
    right->left = left;
    left->right = right;
    for (i = down; i != (DancingLink *)(this); i = i->down) {
        for (j = i->right; j != i; j = j->right) {
            j->down->up = j->up;
            j->up->down = j->down;
            j->colHeader->count--;
        }
    }
}

void DancingLinkColumn::uncover() {
    DancingLink *i;
    DancingLink *j;
    isCoverd = false;
    for (i = up; i != (DancingLink *)(this); i = i->up) {
        for (j = i->left; j != i; j = j->left) {
            j->colHeader->count++;
            j->down->up = j;
            j->up->down = j;
        }
    }
    right->left = (this);
    left->right = (this);
}


sudoku::DancingLinkTable::DancingLinkTable(bool should_randomize):
root(std::make_shared<DancingLink>()),
colHeaders(std::make_shared<std::array<DancingLinkColumn,eConstraints>>()),
buffer(std::make_shared<std::array<DancingLink, eBufferSize>>()){
    generateLinks(should_randomize);
}

/**
 * @brief calculate the constraint columns for a given row, columns and number
 * 
 * @param columns array that gets filled with the correct values
 * @param row 0-(eSize - 1)
 * @param col 0-(eSize - 1)
 * @param num 0-(eSize - 1)
 */
void sudoku::calculateConstraintColumns(int columns[eConstraintTypes], int row, int col, int num) {
    const int box_idx = eBoxSize * (row / eBoxSize) + (col / eBoxSize);
    columns[eConstraintCell] = (row * eSize) + col;
    columns[eConstraintRow] = eBoardSize + (row * eSize) + num;
    columns[eConstraintCol] = (eBoardSize * 2) + (col * eSize) + num;
    columns[eConstraintBox] = (eBoardSize * 3) + (box_idx * eSize) + num;
}

void sudoku::DancingLinkTable::generateLinks(bool should_randomize) {
    {
        DancingLinkColumn *current, *next;

        // Creating initial link
        // current->colHeader = current;

        root->up = root.get();
        root->down = root.get();
        root->count = 0;

        // linking colummn headers
        current = (DancingLinkColumn *)root.get();
        for (auto &i : *(colHeaders.get())) {
        //for (auto i = 0; i < sudoku::eConstraints; i++) {
            //next = &colHeaders.get()[i];
            next = &i;
            current->right = next;
            next->left = current;
            current = next;
            current->up = current;
            current->down = current;
            current->colHeader = current;
            current->count = 0;
            current->isCoverd = false;
        }
        // closing the loop
        current->right = root.get();
        root->left = current;
    }

    int buffer_idx = 0;
    DancingLink *current, *next;
    // creating rows in constraint table
    for (auto row = 0; row < eSize; row++) {
        for (auto col = 0; col < eSize; col++) {
            for (auto num = 0; num < eSize; num++) {
                int constraints[eConstraintTypes];
                calculateConstraintColumns(constraints, row, col, num);
                current = &(buffer->at(buffer_idx + 3));
                int constraintType = 0;
                for (auto &constraint : constraints) {
                    DancingLink *rowToAddTo = &(colHeaders.get()->at(constraint));
                    colHeaders->at(constraint).constraintType = (constraintColTypes)constraintType++;

                    // Randomize for creating puzzles
                    if (should_randomize && colHeaders->at(constraint).count != 0) {
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_int_distribution<> distrib(0, colHeaders->at(constraint).count);
                        auto offset = distrib(gen);
                        for (auto i = 0; i < offset; i++) {
                            rowToAddTo = rowToAddTo->up;
                        }
                    }

                    next = &buffer->at(buffer_idx);
                    current->right = next;
                    next->left = current;

                    current = next;
                    current->down = rowToAddTo;
                    current->up = current->down->up;
                    current->down->up->down = current;
                    current->down->up = current;

                    current->count = (row * eBoardSize) + (col * eSize) + num;

                    current->colHeader = &colHeaders->at(constraint);
                    colHeaders->at(constraint).count++;
                    buffer_idx++;
                }
            }
        }
    }
}

void sudoku::cover_link(DancingLink *link) {
    for (auto col = link->right; col != link; col = col->right) {
        col->colHeader->cover();
    }
}

void sudoku::uncover_link(DancingLink *link) {
    for (auto col = link->left; col != link; col = col->left) {
        col->colHeader->uncover();
    }
}

void sudoku::cover_row(DancingLink *link) {
    link->up->down = link->down;
    link->down->up = link->up;
    link->colHeader->count--;
    for (auto r = link->right; r != link; r = r->right) {
        r->up->down = r->down;
        r->down->up = r->up;
        r->colHeader->count--;
    }
}

void sudoku::uncover_row(DancingLink *link) {
    for (auto l = link->left; l != link; l = l->left) {
        l->down->up = l;
        l->up->down = l;
        l->colHeader->count++;
    }
    link->down->up = link;
    link->up->down = link;
    link->colHeader->count++;
}
