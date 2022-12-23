#include <sstream>
#include <iostream>
#include <algorithm>
#include "Solve.h"
#include "../config.h"

#define CONSTRAINSTS 324
#define CHOICES      729
#define SIZE           9
#define BOX_SIZE       3
#define SATISFIED   0xFF

static void printBoard(puzzle grid) {
    std::stringstream boardStream;
    boardStream << TOPROW << '\n';
    for (auto i = 0; i < 3; i++) {
        boardStream << ROW1 << '\n';
        boardStream << ROW2 << '\n';
        boardStream << ROW1 << '\n';
        boardStream << ROW2 << '\n';
        boardStream << ROW1 << '\n';
        if (i != 2)
            boardStream << ROW3 << '\n';
    }
    boardStream << BOTROW << '\n';
    std::string boardString = boardStream.str();
    int idx = 0;
    for (auto i = 0; i < 9; i++) {
        idx += std::string{TOPROW}.size() + 5;
        for (auto j = 0; j < 9; j++) {
            if (grid[i][j] != 0)
                boardString[idx] = grid[i][j] + '0';
            idx += 6;
        }
    }
    std::cout << boardString << '\n';
}

/**
 * @brief Dancing link which is a quadruply linked list
 * 
 */
struct DancingLink {
    DancingLink *up;
    DancingLink *down;
    DancingLink *left;
    DancingLink *right;
    DancingLink *colHeader;
    int count; // is the column in columns, otherwise is the digit in board
    int row;   // row in board
    int col;   // col in board

    virtual void cover(){}
    virtual void uncover(){}
};

struct DancingLinkHeader : DancingLink {
    void cover() {
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

    void uncover() {
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
};


/**
 * @brief vector to hold the found solution
 * 
 */
static puzzle solution;

/**
 * @brief vector that holds potential solution
 * 
 */
static std::array<DancingLink *, SIZE*SIZE> solutionSet;


/**
 * @brief find the column with the fewest amount of rows
 * 
 * @return ColHeader 
 */
static DancingLink *smallestColumn(DancingLink *root);

/**
 * @brief Gets the solution from the puzzle
 * 
 * @return puzzle 
 */
puzzle Sudoku::getSolution() {
    return solution;
}

/**
 * @brief 
 * 
 * @param depth recursion depth
 * @param solutions number of solutions found
 * @return true if solutions found are greater than one
 * @return false no solutions found
 */
static bool backTrack(int depth, int &solutions, DancingLink *root);

/**
 * @brief Create a Puzzle from a solutionset
 * 
 * @return puzzle 
 */
static puzzle createPuzzle(int depth);

/**
 * @brief Solves a sudoku puzzle
 * 
 * @param grid puzzle to be solved
 * @return true if there is one solution
 * @return false if there are multiple solutions or if no solutions were found
 */
bool Sudoku::solve(puzzle grid) {
    // Clear any previous solutions found
    solution = {};
    
    // Set up buffers that will hold the grid
    DancingLink* root = new DancingLink;
    root->colHeader = root;
    root->row = root->col = root->count = 0;

    std::array<DancingLinkHeader, CONSTRAINSTS> colHeaders;
    std::array<DancingLinkHeader, CHOICES> rowHeaders;
    std::array<DancingLink, CONSTRAINSTS * SIZE> buffer;

    // Storing all the column headers in an array and linking them to each other
    DancingLink *current = root;
    DancingLink *next;
    for (auto &i : colHeaders) {
        next = &i;
        current->right = next;
        next->left = current;

        current = next;

        current->up = current;
        current->down = current;
        current->colHeader = current;
    }
    // Completing the loop
    current->right = root;
    root->left = current;

    // Doing the same for rowHeaders
    current = root;
    for (auto &i : rowHeaders) {
        next = &i;
        current->down = next;
        next->up = current;

        current = next;

        current->left = current;
        current->right = current;
        current->colHeader = root;
    }
    // Completing the loop
    current->down = root;
    root->up = current;
    root->count = CHOICES;

    // building the table
    size_t buffer_idx = 0;
    size_t row_idx = 0;
    for (auto row = 0; row < SIZE; row++) {
        for (auto col = 0; col < SIZE; col++) {
            for (auto num = 0; num < SIZE; num++) {
                int box_idx = BOX_SIZE * (row / BOX_SIZE) + (col / BOX_SIZE);

                int constraints[4];
                constraints[0] = BOX_SIZE * SIZE * num + row;
                constraints[1] = BOX_SIZE * SIZE * num + SIZE + col;
                constraints[2] = BOX_SIZE * SIZE * num + 2 * SIZE + box_idx;
                constraints[3] = BOX_SIZE * SIZE * SIZE + (col + SIZE * row);

                // Setting values so that grid can be translated back into a puzzle
                rowHeaders[row_idx].row = row;
                rowHeaders[row_idx].col = col;
                rowHeaders[row_idx].count = num + 1;
                current = &rowHeaders[row_idx];
                for (auto i = 0; i < 4; i++) {
                    next = &buffer[buffer_idx];
                    current->right = next;
                    next->left = current;

                    current = next;
                    current->down = &colHeaders[constraints[i]];
                    current->up = current->down->up;
                    current->down->up->down = current;
                    current->down->up = current;

                    current->row = row;
                    current->col = col;
                    current->count = num + 1;

                    current->colHeader = &colHeaders[constraints[i]];
                    colHeaders[constraints[i]].count++;

                    buffer_idx++;
                }
                // Completing the loop
                current->right = &rowHeaders[row_idx];
                rowHeaders[row_idx].left = current;
                row_idx++;
            }
        }
    }

    int startDepth = 0;
    // Adding the givens in grid to solutionSet
    for (auto row = 0; row < grid.size(); row++) {
        for (auto col = 0; col < grid[0].size(); col++) {
            if (grid[row][col] == 0) continue;
            auto num = grid[row][col] - 1;
            row_idx = (row * SIZE * SIZE) + (col * SIZE) + num;
            solutionSet[startDepth++] = &rowHeaders[row_idx];

            int box_idx = BOX_SIZE * (row / BOX_SIZE) + (col / BOX_SIZE);

            int constraints[4];
            constraints[0] = BOX_SIZE * SIZE * num + row;
            constraints[1] = BOX_SIZE * SIZE * num + SIZE + col;
            constraints[2] = BOX_SIZE * SIZE * num + 2 * SIZE + box_idx;
            constraints[3] = BOX_SIZE * SIZE * SIZE + (col + SIZE * row);

            for (auto &con : constraints) {
                colHeaders[con].cover();
            }
        }
    }

    // Solving the board
    int solutions = 0;
    bool foundSolution = backTrack(startDepth, solutions, root);
    delete root;
    return foundSolution && solutions == 1;
}

static bool backTrack(int depth, int &solutions, DancingLink *root) {
    if (root->right == root) {
        solutions++;
        solution = createPuzzle(depth);
        printBoard(solution);
        return true;
    }

    puzzle current_step = createPuzzle(depth);
    printBoard(current_step);

    DancingLink *col = smallestColumn(root);

    DancingLink *row;
    DancingLink *cur_col;

    col->cover();
    for (row = col->down; row != col; row=row->down) {
        solutionSet[depth] = row;
        for (cur_col = row->right; cur_col != row; cur_col = cur_col->right) {
            cur_col->colHeader->cover();
        }
        if (backTrack(depth + 1, solutions, root) && solutions > 2) {
            return true;
        }
        for (cur_col = row->left; cur_col != row; cur_col = cur_col->left) {
            cur_col->colHeader->uncover();
        }
    }
    col->uncover();
    return false;
}

static puzzle createPuzzle(int depth) {
    puzzle ret = {};
    for (auto i = 0; i < depth; i++) {
        DancingLink *row = solutionSet[i];
        ret[row->row][row->col] = row->count;
    }
    return ret;
}

static DancingLink *smallestColumn(DancingLink *root) {
    DancingLink *current;
    DancingLink *ret;
    int min = 0xFFFF;
    for (current = root->right; current != root; current = current->right) {
        if (current->count < min) {
            min = current->count;
            ret = current;
            if (min == 0)
                break;
        }
    }
    return ret;
}
