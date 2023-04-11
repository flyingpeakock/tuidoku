#include <algorithm>
#include <random>
#include "Sudoku.h"

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
    int count; // is the column in columns, otherwise is the position and value in the board

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
bool backTrack(int depth, int &solutions, DancingLink *root, Sudoku::puzzle &grid, DancingLink **solutionSet, unsigned int *difficulty);

/**
 * @brief Create a Puzzle from a solutionset
 * 
 * @return Sudoku::puzzle 
 */
static void createPuzzle(int depth, Sudoku::puzzle &grid, DancingLink **solutionSet);

/**
 * @brief calculate the constraint columns for a given row, columns and number
 * 
 * @param columns array that gets filled with the correct values
 * @param row 0-(SIZE - 1)
 * @param col 0-(SIZE - 1)
 * @param num 0-(SIZE - 1)
 */
static void calculateConstraintColumns(int columns[4], int row, int col, int num);

/**
 * @brief Solves a sudoku puzzle
 * 
 * @param grid puzzle to be solved
 * @return true if there is one solution
 * @return false if there are multiple solutions or if no solutions were found
 */
bool Sudoku::solve(puzzle &grid, bool randomize, unsigned int &difficulty) {
    // Set up buffers that will hold the grid
    DancingLink* root = new DancingLink;
    root->colHeader = root;
    root->up = root;
    root->down = root;

    DancingLink colHeaders[CONSTRAINTS];
    DancingLink buffer[CONSTRAINTS * SIZE];

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
        current->count = 0;
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
                int constraints[4];
                calculateConstraintColumns(constraints, row, col, num);

                current = &buffer[buffer_idx + 3]; // since all rows have 4 columns we can start the loop here
                for (auto i = 0; i < 4; i++) {
                    // Randomly place the row in the column if creating a new puzzle
                    DancingLink *rowToAddTo = &colHeaders[constraints[i]];
                    if (randomize && &colHeaders[constraints[i]].count != 0) {
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_int_distribution<> distrib(0, colHeaders[constraints[i]].count);
                        auto offset_max = distrib(gen);
                        for (auto offset = 0; offset < offset_max; offset++) {
                            rowToAddTo = rowToAddTo->up;
                        }
                    }

                    next = &buffer[buffer_idx];
                    current->right = next;
                    next->left = current;

                    current = next;
                    current->down = rowToAddTo;
                    current->up = current->down->up;
                    current->down->up->down = current;
                    current->down->up = current;

                    current->count = (row * SIZE * SIZE) + (col * SIZE) + num;

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
            int constraints[4];
            calculateConstraintColumns(constraints, row, col, num);
            for (auto &con : constraints) {
                colHeaders[con].cover();
            }
        }
    }

    // Creating a buffer that holds rows in the solution set
    DancingLink *solutionSet[SIZE * SIZE];

    // Creating a buffer that holds "difficulty" of each move
    unsigned int difficultySet[SIZE * SIZE];

    // Solving the board
    int solutions = 0;
    bool foundSolution = backTrack(0, solutions, root, grid, solutionSet, difficultySet);
    delete root;
    return solutions == 1;
}

bool Sudoku::solve(puzzle &grid) {
    unsigned int trash;
    return solve(grid, false, trash);
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
bool backTrack(int depth, int &solutions, DancingLink *root, Sudoku::puzzle &grid, DancingLink **solutionSet, unsigned int *difficulty) {
    if (root->right == root) {
        solutions++;
        createPuzzle(depth, grid, solutionSet);
        return true;
    }

    DancingLink *col = smallestColumn(root);
    DancingLink *cur_col;

    col->cover();
    difficulty[depth] = col->count;
    for (DancingLink *row = col->down; row != col; row = row->down) {
        solutionSet[depth] = row;
        for (cur_col = row->right; cur_col != row; cur_col = cur_col->right) {
            cur_col->colHeader->cover();
        }
        if (backTrack(depth + 1, solutions, root, grid, solutionSet, difficulty) && solutions > 1) {
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
void createPuzzle(int depth, Sudoku::puzzle &grid, DancingLink **solutionSet) {
    for (auto d = 0; d < depth; d++) {
        DancingLink *row = solutionSet[d];
        int i = row->count / (Sudoku::SIZE * Sudoku::SIZE);
        int j = (row->count % (Sudoku::SIZE * Sudoku::SIZE)) / Sudoku::SIZE;
        int num = (row->count % Sudoku::SIZE) + 1;
        grid[i][j] = num;
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

static void calculateConstraintColumns(int columns[4], int row, int col, int num) {
    const int box_idx = BOX_SIZE * (row / BOX_SIZE) + (col / BOX_SIZE);
    const int constraintSection = Sudoku::SIZE * Sudoku::SIZE;
    columns[0] = (row * Sudoku::SIZE) + col;
    columns[1] = constraintSection + (row * Sudoku::SIZE) + num;
    columns[2] = (constraintSection * 2) + (col * Sudoku::SIZE) + num;
    columns[3] = (constraintSection * 3) + (box_idx * Sudoku::SIZE) + num;
}