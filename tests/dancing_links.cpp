#include "dancing_links.h"
#include <gtest/gtest.h>

TEST(dancing_links, calculate_constraint_columns) {
    using namespace Sudoku;
    int constraints[eConstraintTypes];
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

    for (current = table_f.root->right; current != table_f.root.get(); current = current->right) {
        EXPECT_EQ(current->count, Sudoku::eSize);
        col = current;
        for (auto i = 0; i <= Sudoku::eSize; i++) {
            col = col->down;
        }
        EXPECT_EQ(current, col);
    }

    for (current = table_t.root->right; current != table_t.root.get(); current = current->right) {
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
        for (col = t.root->right; col != t.root.get(); col = col->right) {
            row = col->down;
            for (auto i = 0; i < Sudoku::eConstraintTypes; i++) {
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
        for (col = t.root->right; col != t.root.get(); col = col->right) {
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
        //for (auto &link : tables[i].buffer) {
        for (auto j = 0; j < Sudoku::eBufferSize; j++) {
            auto link = tables[i].buffer.get() + j;
            results[i][Sudoku::getRowFromLink(link)][Sudoku::getColFromLink(link)]++;
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
    //for (auto &link : table_f.colHeaders) {
    for (auto i = 0; i < Sudoku::eConstraints; i++) {
        auto link = table_f.colHeaders.get() + i;
        EXPECT_EQ(link, current);
        current = current->right;
    }

    current = &table_t.colHeaders[0];
    //for (auto &link : table_t.colHeaders) {
    for (auto i = 0; i < Sudoku::eConstraints; i++) {
        auto link = table_t.colHeaders.get() + i;
        EXPECT_EQ(link, current);
        current = current->right;
    }
}

TEST(dancing_links, none_covered_after_solve) {
    Sudoku::DancingLinkTable tables[2] = {true, false};
    bool bools[2] = {true, false};
    for (auto &table : tables) {
        for (auto b : bools) {
            Sudoku::solve(table, b);
            int counter = 0;
            for (auto col = table.root->right; col != table.root.get(); col = col->right) {
                counter++;
            }
            EXPECT_EQ(counter, Sudoku::eConstraints);
        }
    }
}

TEST(dancing_links, solve_does_not_uncover_covered) {
    Sudoku::DancingLinkTable table(false);
    int constraints[Sudoku::eConstraintTypes];
    Sudoku::calculateConstraintColumns(constraints, Sudoku::eConstraintTypes, 6, 7);

    for (auto c : constraints) {
        table.colHeaders[c].cover();
    }
    Sudoku::solve(table, false);
    for (auto c : constraints) {
        EXPECT_NE(table.colHeaders[c].right->left, &table.colHeaders[c]);
    }
}

TEST(dancing_links, each_col_has_9_count) {
    Sudoku::DancingLinkTable table(true);
    for (auto i = table.root->right; i != table.root.get(); i = i->right) {
        EXPECT_EQ(i->count, Sudoku::eSize);
    }

    Sudoku::DancingLinkTable generatedTable = Sudoku::generate();
    for (auto i = table.root->right; i != table.root.get(); i = i->right) {
        EXPECT_LE(i->count, Sudoku::eSize);
    }
}

TEST(dancing_links, generate_doesnt_break_columns) {
    Sudoku::DancingLinkTable table = Sudoku::generate(Sudoku::eAny);
    int i = 0;
    bool seen_root = false;
    Sudoku::DancingLink *current = table.root->right;
    for (i; i < Sudoku::eConstraints; i++) {
        if (current == table.root.get()) {
            seen_root = true;
            break;
        }
        current = current->right;
    }
    EXPECT_TRUE(seen_root);
}

TEST(dancing_links, each_generated_link_has_valid_pointers) {
    Sudoku::DancingLinkTable table = Sudoku::generate(Sudoku::eAny);
    ASSERT_NO_THROW(table.root->right);
    for (auto col = table.root->right; col != table.root.get(); col = col->right) {
        ASSERT_NO_THROW(col->down);
        for (auto row = col->down; row != col; row = row->down) {
            ASSERT_NO_THROW(row->count);
        }
        ASSERT_NO_THROW(col->right);
    }
}

TEST(dancing_links, table_copy_refers_to_same_links) {
    auto table = Sudoku::generate();
    auto table_copy = table;
    EXPECT_EQ(table.root, table_copy.root);
    EXPECT_EQ(table.root.get(), table_copy.root.get());

    EXPECT_EQ(table.buffer, table_copy.buffer);
    for (auto i = 0; i < Sudoku::eBufferSize; i++) {
        EXPECT_EQ(table.buffer.get() + i, table_copy.buffer.get() + i);
    }

    EXPECT_EQ(table.colHeaders, table_copy.colHeaders);
    for (auto i = 0; i < Sudoku::eConstraints; i++) {
        EXPECT_EQ(table.colHeaders.get() + i, table_copy.colHeaders.get() + i);
    }

    EXPECT_EQ(table.current, table_copy.current);
    EXPECT_EQ(table.solution, table_copy.solution);

    Sudoku::DancingLink *otherCol = table_copy.root->right;
    for (auto *col = table.root->right; col != table.root.get(); col = col->right) {
        EXPECT_EQ(col, otherCol);
        col->colHeader->cover();
        Sudoku::cover_link(col->down);
        EXPECT_EQ(col->right->left, otherCol->left);
        EXPECT_EQ(col->down->left->up, otherCol->down->left->up);
        Sudoku::uncover_link(col->down);
        col->colHeader->uncover();
        auto otherRow = otherCol->down;
        for (auto *row = col->down; row != col; row = row->down) {
            EXPECT_EQ(row, otherRow);
            EXPECT_EQ(row->colHeader, otherRow->colHeader);
            otherRow = otherRow->down;
        }
        otherCol = otherCol->right;
    }
}
