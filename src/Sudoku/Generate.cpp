#include "Sudoku.h"
#include "../Play.h"
#include "../HumanSolve.h"
#include <algorithm>
#include <chrono>
#include <random>

static Sudoku::puzzle removeGivens(Sudoku::puzzle filled);

static Sudoku::difficulty gradePuzzle(Sudoku::puzzle &grid, Sudoku::difficulty maxDiff);

Sudoku::puzzle Sudoku::generate() {
    return generate(ANY);
}

Sudoku::puzzle Sudoku::generate(difficulty diff) {
    puzzle grid = {};
    solve(grid, true);

    grid = removeGivens(grid);

    if (diff == ANY)
    {
        return grid;
    }

    Sudoku::difficulty this_diff = gradePuzzle(grid, diff);
    if (diff != this_diff)
        return generate(diff);

    return grid;
}

static Sudoku::puzzle removeGivens(Sudoku::puzzle filled) {
    Sudoku::puzzle grid = filled;
    struct Cell
    {
        int row;
        int col;
    };

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::array<Cell, Sudoku::SIZE*Sudoku::SIZE> cells;
    int count = 0;
    for (auto i = 0; i < Sudoku::SIZE; i++) {
        for (auto j = 0; j < Sudoku::SIZE; j++) {
            cells[count].row = i;
            cells[count].col = j;
            count++;
        }
    }

    // Shuffle to randomly remove positions
    shuffle(cells.begin(), cells.end(), std::default_random_engine(seed));

    Sudoku::puzzle copy;
    for (const auto &cell : cells) {
        int removed = grid[cell.row][cell.col];
        grid[cell.row][cell.col] = 0;
        copy = grid;
        bool isUnique = Sudoku::solve(copy, false);
        if (!isUnique) {
            // Removal made it a bad move, put it back
            grid[cell.row][cell.col] = removed;
        }
    }
    return grid;
}

static Sudoku::difficulty gradePuzzle(Sudoku::puzzle &grid, Sudoku::difficulty maxDiff) {
    // Ranking the board using the human solver
    Play board({}, grid, NULL);
    board.autoPencil();
    Hint hint = solveHuman(board);
    std::vector<Hint> allHints;
    allHints.push_back(hint);
    while (hint.moves.size() > 0) {
        for (auto &move : hint.moves) {
            move(&board);
        }
        hint = solveHuman(board);
        allHints.push_back(hint);
    }

    // Couldn't solve
    if (!board.isWon()) {
        return Sudoku::HIGHEST;
    }

    Sudoku::difficulty highestDifficulty = Sudoku::BEGINNER;
    for (const auto h : allHints) {
        for (const auto m : h.moves) {
            if (m.difficulty > highestDifficulty) {
                highestDifficulty = m.difficulty;
            }

            // filling in more difficult moves
            if (m.difficulty > maxDiff) {
                grid[m.row][m.col] = m.val;
                return gradePuzzle(grid, maxDiff);
            }
        }
    }
    return highestDifficulty;
}
