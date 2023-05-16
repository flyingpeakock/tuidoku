#include "dancing_links.h"
#include <gtest/gtest.h>

TEST(dancing_links, calculate_constraint_columns) {
    using namespace sudoku;
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
    sudoku::DancingLinkTable table_f(false);
    sudoku::DancingLinkTable table_t(true);
    sudoku::DancingLink *current;
    sudoku::DancingLink *col;

    for (current = table_f.root->right; current != table_f.root.get(); current = current->right) {
        EXPECT_EQ(current->count, sudoku::eSize);
        col = current;
        for (auto i = 0; i <= sudoku::eSize; i++) {
            col = col->down;
        }
        EXPECT_EQ(current, col);
    }

    for (current = table_t.root->right; current != table_t.root.get(); current = current->right) {
        EXPECT_EQ(current->count, sudoku::eSize);
        col = current;
        for (auto i = 0; i <= sudoku::eSize; i++) {
            col = col->down;
        }
        EXPECT_EQ(current, col);
    }
}

TEST(dancing_links, each_row_has_four_columns) {
    sudoku::DancingLink *row;
    sudoku::DancingLink *col;
    sudoku::DancingLinkTable tables[] = { sudoku::DancingLinkTable(false), sudoku::DancingLinkTable(true) };

    for (auto &t : tables) {
        for (col = t.root->right; col != t.root.get(); col = col->right) {
            row = col->down;
            for (auto i = 0; i < sudoku::eConstraintTypes; i++) {
                row = row->right;
            }
            EXPECT_EQ(row, col->down);
        }
    }
}

TEST(dancing_links, each_row_has_same_count) {
    sudoku::DancingLink *row;
    sudoku::DancingLink *col;
    int count;
    sudoku::DancingLinkTable tables[] = { sudoku::DancingLinkTable(false), sudoku::DancingLinkTable(true) };

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
    sudoku::DancingLinkTable tables[] = { sudoku::DancingLinkTable(false), sudoku::DancingLinkTable(true) };

    int results[2][sudoku::eSize][sudoku::eSize] = {0};

    for (auto i = 0; i < 2; i++) {
        //for (auto &link : tables[i].buffer) {
        for (auto j = 0; j < sudoku::eBufferSize; j++) {
            auto link = &tables[i].buffer->at(j);
            results[i][sudoku::getRowFromLink(link)][sudoku::getColFromLink(link)]++;
        }
    }

    for (auto i : results) {
        for (auto j = 0; j < sudoku::eSize; j++) {
            for (auto k = 0; k < sudoku::eSize; k++) {
                EXPECT_EQ(i[j][k], 36);
            }
        }
    }
}

TEST(dancing_links, all_columns_are_linked) {
    sudoku::DancingLinkTable table_f(false);
    sudoku::DancingLinkTable table_t(true);
    sudoku::DancingLink *current;

    current = &table_f.colHeaders->at(0);
    //for (auto &link : table_f.colHeaders) {
    for (auto i = 0; i < sudoku::eConstraints; i++) {
        auto link = &table_f.colHeaders->at(i);
        EXPECT_EQ(link, current);
        current = current->right;
    }

    current = &table_t.colHeaders->at(0);
    //for (auto &link : table_t.colHeaders) {
    for (auto i = 0; i < sudoku::eConstraints; i++) {
        auto link = &table_t.colHeaders->at(i);
        EXPECT_EQ(link, current);
        current = current->right;
    }
}

TEST(dancing_links, none_covered_after_solve) {
    sudoku::DancingLinkTable tables[2] = {true, false};
    bool bools[2] = {true, false};
    for (auto &table : tables) {
        for (auto b : bools) {
            sudoku::solve(table, b);
            int counter = 0;
            for (auto col = table.root->right; col != table.root.get(); col = col->right) {
                counter++;
            }
            EXPECT_EQ(counter, sudoku::eConstraints);
        }
    }
}

TEST(dancing_links, solve_does_not_uncover_covered) {
    sudoku::DancingLinkTable table(false);
    int constraints[sudoku::eConstraintTypes];
    sudoku::calculateConstraintColumns(constraints, sudoku::eConstraintTypes, 6, 7);

    for (auto c : constraints) {
        table.colHeaders->at(c).cover();
    }
    sudoku::solve(table, false);
    for (auto c : constraints) {
        EXPECT_NE(table.colHeaders->at(c).right->left, &table.colHeaders->at(c));
    }
}

TEST(dancing_links, each_col_has_9_count) {
    sudoku::DancingLinkTable table(true);
    for (auto i = table.root->right; i != table.root.get(); i = i->right) {
        EXPECT_EQ(i->count, sudoku::eSize);
    }

    sudoku::DancingLinkTable generatedTable = sudoku::generate();
    for (auto i = table.root->right; i != table.root.get(); i = i->right) {
        EXPECT_LE(i->count, sudoku::eSize);
    }
}

TEST(dancing_links, generate_doesnt_break_columns) {
    sudoku::DancingLinkTable table = sudoku::generate(sudoku::eAny);
    bool seen_root = false;
    sudoku::DancingLink *current = table.root->right;
    for (auto i = 0; i < sudoku::eConstraints; i++) {
        if (current == table.root.get()) {
            seen_root = true;
            break;
        }
        current = current->right;
    }
    EXPECT_TRUE(seen_root);
}

TEST(dancing_links, each_generated_link_has_valid_pointers) {
    sudoku::DancingLinkTable table = sudoku::generate(sudoku::eAny);
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
    auto table = sudoku::generate();
    auto table_copy = table;
    EXPECT_EQ(table.root, table_copy.root);
    EXPECT_EQ(table.root.get(), table_copy.root.get());

    EXPECT_EQ(table.buffer, table_copy.buffer);
    for (auto i = 0; i < sudoku::eBufferSize; i++) {
        EXPECT_EQ(table.buffer.get() + i, table_copy.buffer.get() + i);
    }

    EXPECT_EQ(table.colHeaders, table_copy.colHeaders);
    for (auto i = 0; i < sudoku::eConstraints; i++) {
        EXPECT_EQ(table.colHeaders.get() + i, table_copy.colHeaders.get() + i);
    }

    EXPECT_EQ(table.current, table_copy.current);
    EXPECT_EQ(table.solution, table_copy.solution);

    sudoku::DancingLink *otherCol = table_copy.root->right;
    for (auto *col = table.root->right; col != table.root.get(); col = col->right) {
        EXPECT_EQ(col, otherCol);
        col->colHeader->cover();
        sudoku::cover_link(col->down);
        EXPECT_EQ(col->right->left, otherCol->left);
        EXPECT_EQ(col->down->left->up, otherCol->down->left->up);
        sudoku::uncover_link(col->down);
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
