#include "GeneratorStatistics.h"
#include "../src/Generator.h"
#include "../src/HumanSolve.h"
#include <iostream>

static int countEmptyCells(const Board &board) {
    int count = 0;
    auto grid = board.getPlayGrid();
    for (auto &row : grid) {
        for (auto &cell : row) {
            if (cell == 0) count++;
        }
    }
    return count;
}

SingleStats gradeBoard(Board &board) {
    static int count = 0;
    count++;
    SingleStats stats = {};
    stats.emptyGrids = countEmptyCells(board);

    Hint hint = solveHuman(board);

    std::cout << "--> Solving board " << count << '\n';
    while(hint.moves.size() > 0) {
        if (hint.difficulty > stats.difficulty) {
            stats.difficulty = hint.difficulty;
        }
        for (auto &move : hint.moves) {
            move(&board);
            stats.movesToSolve++;
        }
        hint = solveHuman(board);
    }
    std::cout << "---> Difficulty " << stats.difficulty << '\n';
    std::cout << "---> Empty cells " << stats.emptyGrids << '\n';
    std::cout << "---> Moves to solve " << stats.movesToSolve << "\n";
    return stats;
}

void accumulateBoardGrades(int count) {
    long cum_difficulty = 0;
    long cum_empty_cells = 0;
    long cum_moves_to_solve = 0;

    for (auto i = 0; i < count; i++) {
        std::cout << "\nGenerating board " << i + 1 << '\n';
        Board board = Generator().createBoard();
        board.autoPencil();
        auto stats = gradeBoard(board);
        cum_difficulty += stats.difficulty;
        cum_empty_cells += stats.emptyGrids;
        cum_moves_to_solve += stats.movesToSolve;
        std::cout << "\n--> Average difficulty " << (float)(cum_difficulty) / (i + 1) << '\n';
        std::cout << "--> Average empty cells " << (float)(cum_empty_cells) / (i + 1) << '\n';
        std::cout << "--> Average moves to solve " << (float)(cum_moves_to_solve) / (i + 1) << '\n';
    }
}