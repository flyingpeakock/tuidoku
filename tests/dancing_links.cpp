#include "dancing_links.h"
#include <gtest/gtest.h>

TEST(dancing_links, calculate_constraint_columns) {
    using namespace Sudoku;
    int constraints[4];
    for (auto i = 0; i < eSize; i++) {
        for (auto j = 0; j < eSize; j++) {
            for (auto num = 0; num < eSize; num++) {
                int box = 3 * (i / 3) + (j / 3);
                calculateConstraintColumns(constraints, i, j, num);
                EXPECT_EQ(constraints[0], (i * eSize) + j);
                EXPECT_EQ(constraints[1], (eSize * eSize) + (i * eSize) + num);
                EXPECT_EQ(constraints[2], (eSize * eSize) + (eSize * eSize) + (j * eSize) + num);
                EXPECT_EQ(constraints[3], (eSize * eSize) + (eSize * eSize) + (eSize * eSize) + (box * eSize) + num);
            }
        }
    }
}

TEST(dancing_links, each_col_has_eSize_rows) {
    Sudoku::DancingLinkTable table_f(false);
    Sudoku::DancingLinkTable table_t(true);
    Sudoku::DancingLink *current;
    Sudoku::DancingLink *col;

    for (current = table_f.root.right; current != &table_f.root; current = current->right) {
        EXPECT_EQ(current->count, Sudoku::eSize);
        col = current;
        for (auto i = 0; i <= Sudoku::eSize; i++) {
            col = col->down;
        }
        EXPECT_EQ(current, col);
    }

    for (current = table_t.root.right; current != &table_t.root; current = current->right) {
        EXPECT_EQ(current->count, Sudoku::eSize);
        col = current;
        for (auto i = 0; i <= Sudoku::eSize; i++) {
            col = col->down;
        }
        EXPECT_EQ(current, col);
    }
}

TEST(dancing_links, each_row_has_four_columns) {
    Sudoku::DancingLink *row;
    Sudoku::DancingLink *col;
    Sudoku::DancingLinkTable tables[] = { Sudoku::DancingLinkTable(false), Sudoku::DancingLinkTable(true) };

    for (auto &t : tables) {
        for (col = t.root.right; col != &t.root; col = col->right) {
            row = col->down;
            for (auto i = 0; i < 4; i++) {
                row = row->right;
            }
            EXPECT_EQ(row, col->down);
        }
    }
}

TEST(dancing_links, each_row_has_same_count) {
    Sudoku::DancingLink *row;
    Sudoku::DancingLink *col;
    int count;
    Sudoku::DancingLinkTable tables[] = { Sudoku::DancingLinkTable(false), Sudoku::DancingLinkTable(true) };

    for (auto &t : tables) {
        for (col = t.root.right; col != &t.root; col = col->right) {
            count = col->down->count;
            for (row = col->down->right; row != col->down; row = row->right) {
                EXPECT_EQ(row->count, count);
            }
        }
    }
}

TEST(dancing_links, each_cell_has_36_links) {
    Sudoku::DancingLinkTable tables[] = { Sudoku::DancingLinkTable(false), Sudoku::DancingLinkTable(true) };

    int results[2][Sudoku::eSize][Sudoku::eSize] = {0};

    for (auto i = 0; i < 2; i++) {
        for (auto &link : tables[i].buffer) {
            results[i][Sudoku::getRowFromLink(&link)][Sudoku::getColFromLink(&link)]++;
        }
    }

    for (auto i : results) {
        for (auto j = 0; j < Sudoku::eSize; j++) {
            for (auto k = 0; k < Sudoku::eSize; k++) {
                EXPECT_EQ(i[j][k], 36);
            }
        }
    }
}

TEST(dancing_links, all_columns_are_linked) {
    Sudoku::DancingLinkTable table_f(false);
    Sudoku::DancingLinkTable table_t(true);
    Sudoku::DancingLink *current;

    current = &table_f.colHeaders[0];
    for (auto &link : table_f.colHeaders) {
        EXPECT_EQ(&link, current);
        current = current->right;
    }

    current = &table_t.colHeaders[0];
    for (auto &link : table_t.colHeaders) {
        EXPECT_EQ(&link, current);
        current = current->right;
    }
}

TEST(dancing_links, repair_links) {
    Sudoku::DancingLinkTable table(false);
    Sudoku::solve(&table, false);
    Sudoku::repairLinks(&table);
    Sudoku::DancingLink *current = &table.colHeaders[0];
    for (auto &link : table.colHeaders) {
        EXPECT_EQ(&link, current);
        current = current->right;
    }

    Sudoku::DancingLink *col;
    Sudoku::DancingLink *row;
    for (col = table.root.right; col != &table.root; col = col->right) {
        row = col->down;
        for (auto i = 0; i < 4; i++) {
            row = row->right;
        }
        EXPECT_EQ(row, col->down);
    }
}

TEST(dancing_links, none_covered_after_solve) {
    Sudoku::DancingLinkTable tables[2] = {true, false};
    bool bools[2] = {true, false};
    for (auto &table : tables) {
        for (auto b : bools) {
            Sudoku::solve(&table, b);
            size_t counter = 0;
            for (auto col = table.root.right; col != &table.root; col = col->right) {
                counter++;
            }
            EXPECT_EQ(counter, Sudoku::eConstraints);
        }
    }
}
