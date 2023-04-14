#include "Sudoku.h"
#include <random>
#include "DancingLink.h"

using namespace Sudoku;

static void generateLinks(DancingLinkTable *table, bool should_randomize);

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


Sudoku::DancingLinkTable::DancingLinkTable(bool should_randomize) {
    generateLinks(this, should_randomize);
}

/**
 * @brief calculate the constraint columns for a given row, columns and number
 * 
 * @param columns array that gets filled with the correct values
 * @param row 0-(eSize - 1)
 * @param col 0-(eSize - 1)
 * @param num 0-(eSize - 1)
 */
void Sudoku::calculateConstraintColumns(int columns[4], int row, int col, int num) {
    const int box_idx = eBoxSize * (row / eBoxSize) + (col / eBoxSize);
    columns[0] = (row * eSize) + col;
    columns[1] = eBoardSize + (row * eSize) + num;
    columns[2] = (eBoardSize * 2) + (col * eSize) + num;
    columns[3] = (eBoardSize * 3) + (box_idx * eSize) + num;
}

static void generateLinks(DancingLinkTable *table, bool should_randomize) {
    DancingLink *current = &table->root;
    DancingLink *next;
    int buffer_idx = 0;

    // Creating initial link
    current->colHeader = current;
    current->up = current;
    current->down = current;
    current->count = 0;

    // linking colummn headers
    for (auto &i : table->colHeaders) {
        next = &i;
        current->right = next;
        next->left = current;
        current = next;
        current->up = current;
        current->down = current;
        current->colHeader = current;
        current->count = 0;
    }
    // closing the loop
    current->right = &table->root;
    table->root.left = current;

    // creating rows in constraint table
    for (auto row = 0; row < eSize; row++) {
        for (auto col = 0; col < eSize; col++) {
            for (auto num = 0; num < eSize; num++) {
                int constraints[4];
                calculateConstraintColumns(constraints, row, col, num);
                current = &table->buffer[buffer_idx + 3];
                for (auto &constraint : constraints) {
                    DancingLink *rowToAddTo = &table->colHeaders[constraint];

                    // Randomize for creating puzzles
                    if (should_randomize && table->colHeaders[constraint].count != 0) {
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_int_distribution<> distrib(0, table->colHeaders[constraint].count);
                        auto offset = distrib(gen);
                        for (auto i = 0; i < offset; i++) {
                            rowToAddTo = rowToAddTo->up;
                        }
                    }

                    next = &table->buffer[buffer_idx];
                    current->right = next;
                    next->left = current;

                    current = next;
                    current->down = rowToAddTo;
                    current->up = current->down->up;
                    current->down->up->down = current;
                    current->down->up = current;

                    current->count = (row * eBoardSize) + (col * eSize) + num;

                    current->colHeader = &table->colHeaders[constraint];
                    table->colHeaders[constraint].count++;
                    buffer_idx++;
                }
            }
        }
    }
}