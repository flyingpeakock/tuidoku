#include "Sudoku.h"
#include <random>

using namespace Sudoku;

void DancingLink::cover() {
    DancingLink *i;
    DancingLink *j;
    right->left = left;
    left->right = right;

    for (i = down; i != (this); i = i->down) {
        for (j = i->right; j != i; j = j->right) {
            j->down->up = j->up;
            j->up->down = j->down;
            j->colHeader->count--;
        }
    }
}

void DancingLink::uncover() {
            DancingLink *i;
            DancingLink *j;
            for (i = up; i != (this); i = i->up) {
                for (j = i->left; j != i; j = j->left) {
                    j->colHeader->count++;
                    j->down->up = j;
                    j->up->down = j;
                }
            }
            right->left = (this);
            left->right = (this);
}

DancingLinkTables::DancingLinkTables(bool randomize) : should_randomize(randomize) {
    current_idx = 0;
    resetLinks();
}

void DancingLinkTables::resetLinks() {
    DancingLink *current = &root;
    DancingLink *next;
    size_t buffer_idx = 0;

    // Creating initial link
    current->colHeader = current;
    current->up = current;
    current->down = current;

    for (auto i = 0; i < eConstraints; i++) {
        next = &colHeaders[i];
        current->right = next;
        next->left = current;
        current = next;
        current->up = current;
        current->down = current;
        current->colHeader = current;
        current->count = 0;
    }
    // closing the loop
    current->right = &root;
    root.left = current;

    for (auto row = 0; row < eSize; row++) {
        for (auto col = 0; col < eSize; col++) {
            for (auto num = 0; num < eSize; num++) {
                int constraints[4];
                calculateConstraintColumns(constraints, row, col, num);
                current = &buffer[buffer_idx + 3];
                for (auto &constraint : constraints) {
                    DancingLink *rowToAddTo = &colHeaders[constraint];

                    // Randomize for creating puzzles
                    if (should_randomize && colHeaders[constraint].count != 0) {
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_int_distribution<> distrib(0, colHeaders[constraint].count);
                        auto offset = distrib(gen);
                        for (auto i = 0; i < offset; i++) {
                            rowToAddTo = rowToAddTo->up;
                        }
                    }

                    next = &buffer[buffer_idx + 3];
                    current->right = next;
                    next->left = current;

                    current = next;
                    current->down = rowToAddTo;
                    current->up = current->down->up;
                    current->down->up->down = current;
                    current->down->up = current;

                    current->count = (row * eSize * eSize) + (col * eSize) + num;

                    current->colHeader = &colHeaders[constraint];
                    colHeaders[constraint].count++;
                    buffer_idx++;
                }
            }
        }
    }
}

bool DancingLinkTables::isUnique() const {
    return solution_count == 1;
}
