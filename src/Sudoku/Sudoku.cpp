#include "Sudoku.h"

/**
 * @brief calculate the constraint columns for a given row, columns and number
 * 
 * @param columns array that gets filled with the correct values
 * @param row 0-(eSize - 1)
 * @param col 0-(eSize - 1)
 * @param num 0-(eSize - 1)
 */
void Sudoku::calculateConstraintColumns(int columns[4], int row, int col, int num) {
    const int box_idx = Sudoku::eBoxSize * (row / Sudoku::eBoxSize) + (col / Sudoku::eBoxSize);
    const int constraintSection = Sudoku::eSize * Sudoku::eSize;
    columns[0] = (row * Sudoku::eSize) + col;
    columns[1] = constraintSection + (row * Sudoku::eSize) + num;
    columns[2] = (constraintSection * 2) + (col * Sudoku::eSize) + num;
    columns[3] = (constraintSection * 3) + (box_idx * Sudoku::eSize) + num;
}
