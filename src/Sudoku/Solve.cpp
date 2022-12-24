#include <algorithm>
#include <random>
#include <chrono>
#include "Sudoku.h"

//#define BOX_SIZE       3
#define SATISFIED   0xFF

const int CONSTRAINTS = Sudoku::SIZE * Sudoku::SIZE * 4;
const int BOX_SIZE = sqrt(Sudoku::SIZE);
const int CHOICES = Sudoku::SIZE * Sudoku::SIZE * Sudoku::SIZE;


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

/**
 * @brief overrides the virtual methods
 * 
 */
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
 * @brief find the column with the fewest amount of rows
 * 
 * @return ColHeader 
 */
static DancingLink *smallestColumn(DancingLink *root);

/**
 * @brief 
 * 
 * @param depth recursion depth
 * @param solutions number of solutions found
 * @return true if solutions found are greater than one
 * @return false no solutions found
 */
static bool backTrack(int depth, int &solutions, DancingLink *root, Sudoku::puzzle &grid, DancingLink **solutionSet);

/**
 * @brief Create a Puzzle from a solutionset
 * 
 * @return Sudoku::puzzle 
 */
static void createPuzzle(int depth, Sudoku::puzzle &grid, DancingLink **solutionSet);

/**
 * @brief randomly order the rows in a column
 * 
 * @param col 
 * @return std::vector<DancingLink *>  of the randomly ordered rows
 */
static std::vector<DancingLink *> randomlyOrderRows(DancingLink *col);

/**
 * @brief Solves a sudoku puzzle
 * 
 * @param grid puzzle to be solved
 * @return true if there is one solution
 * @return false if there are multiple solutions or if no solutions were found
 */
bool Sudoku::solve(puzzle &grid) {
    // Set up buffers that will hold the grid
    DancingLink* root = new DancingLink;
    root->colHeader = root;
    root->row = root->col = root->count = 0;
    root->up = root;
    root->down = root;

    std::array<DancingLinkHeader, CONSTRAINTS> colHeaders;
    //std::array<DancingLinkHeader, CHOICES> rowHeaders;
    std::array<DancingLink, CONSTRAINTS * Sudoku::SIZE> buffer;

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

                current = &buffer[buffer_idx + 3]; // since all rows have 4 columns we can start the loop here
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
                row_idx++;
            }
        }
    }

    // Covering the clues given in the grid
    for (auto row = 0; row < grid.size(); row++) {
        for (auto col = 0; col < grid[0].size(); col++) {
            if (grid[row][col] == 0) continue;
            auto num = grid[row][col] - 1;
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

    // Creating a buffer that holds rows in the solution set
    DancingLink *solutionSet[SIZE * SIZE];

    // Solving the board
    int solutions = 0;
    bool foundSolution = backTrack(0, solutions, root, grid, solutionSet);
    delete root;
    return solutions == 1;
}

/**
 * @brief Actual DLX algorithm
 * 
 * @param depth current depth, used for creating the board
 * @param solutions number of solutions found
 * @param root root of the constraint grid
 * @return true if a solution is found
 * @return false if no solution is found
 */
static bool backTrack(int depth, int &solutions, DancingLink *root, Sudoku::puzzle &grid, DancingLink **solutionSet) {
    if (root->right == root) {
        solutions++;
        createPuzzle(depth, grid, solutionSet);
        return true;
    }

    DancingLink *col = smallestColumn(root);
    DancingLink *cur_col;

    col->cover();
    std::vector<DancingLink *>randomlyOrderedRows = randomlyOrderRows(col);
    for (DancingLink *row : randomlyOrderedRows) {
        solutionSet[depth] = row;
        for (cur_col = row->right; cur_col != row; cur_col = cur_col->right) {
            cur_col->colHeader->cover();
        }
        if (backTrack(depth + 1, solutions, root, grid, solutionSet) && solutions > 1) {
            return true;
        }
        for (cur_col = row->left; cur_col != row; cur_col = cur_col->left) {
            cur_col->colHeader->uncover();
        }
    }
    col->uncover();
    return false;
}

/**
 * @brief Create a Puzzle from rows in the constraint grid
 * 
 * @param depth that has been searched
 * @param grid to put the pieces into
 */
static void createPuzzle(int depth, Sudoku::puzzle &grid, DancingLink **solutionSet) {
    for (auto i = 0; i < depth; i++) {
        DancingLink *row = solutionSet[i];
        grid[row->row][row->col] = row->count;
    }
}

/**
 * @brief finds the smallest column
 * 
 * @param root 
 * @return DancingLink* to the smallest column
 */
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

/**
 * @brief randomly order the rows seen by a column to be able to generate boards
 * 
 * @param col to search through
 * @return std::vector<DancingLink *> to all the rows in a random order
 */
static std::vector<DancingLink *> randomlyOrderRows(DancingLink *col) {
    std::vector<DancingLink *> ret;
    for (DancingLink *current = col->down; current != col; current = current->down) {
        ret.push_back(current);
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(ret.begin(), ret.end(), std::default_random_engine(seed));
    return ret;
}
