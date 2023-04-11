#include "Sudoku.h"

using namespace Sudoku;

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
//bool backTrack(int depth, int &solutions, DancingLink *root, Sudoku::puzzle &grid, DancingLink **solutionSet, unsigned int *difficulty);
bool backTrack(DancingLinkTables &data);

/**
 * @brief Create a Puzzle from a solutionset
 * 
 * @return Sudoku::puzzle 
 */
static void createPuzzle(int depth, Sudoku::puzzle &grid, DancingLink **solutionSet);


/**
 * @brief Solves a sudoku puzzle
 * 
 * @param grid puzzle to be solved
 * @return true if there is one solution
 * @return false if there are multiple solutions or if no solutions were found
 */
bool Sudoku::solve(puzzle &grid, bool randomize, unsigned int &difficulty) {
    DancingLinkTables data;
    data.current_idx = 0;
    data.solution_idx = -1; // Since the function increments we start at -1
    data.solution_count = 0;

    linkPuzzle(randomize, data.buffer, &data.root, data.colHeaders);
    coverGivens(grid, data.colHeaders);

    // Solving the board
    bool foundSolution = backTrack(data);
    grid = data.createSolutionPuzzle();
    return data.solution_count == 1;
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
//bool backTrack(int depth, int &solutions, DancingLink *root, Sudoku::puzzle &grid, DancingLink **solutionSet, unsigned int *difficulty) {
bool backTrack(DancingLinkTables &data) {
    if (data.root.right == &data.root) {
        data.solution_count++;
        //createPuzzle(depth, grid, solutionSet);
        return true;
    }

    DancingLink *col = smallestColumn(&data.root);
    DancingLink *cur_col;

    col->cover();
    data.solution_idx++;
    for (DancingLink *row = col->down; row != col; row = row->down) {
        data.solution[data.solution_idx] = row;
        for (cur_col = row->right; cur_col != row; cur_col = cur_col->right) {
            cur_col->colHeader->cover();
        }
        if (backTrack(data) && data.solution_count > 1) {
            return true;
        }
        for (cur_col = row->left; cur_col != row; cur_col = cur_col->left) {
            cur_col->colHeader->uncover();
        }
    }
    data.solution_idx--;
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
