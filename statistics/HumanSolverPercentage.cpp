#include "HumanSolverPercentage.h"
#include "../src/HumanSolve.h"
#include "../src/Sudoku/Sudoku.h"
#include <iostream>
#include <chrono>
#include <ctime>

bool HumanSolveUntilFailed(Play &board) {
    Hint hint = solveHuman(board);

    // get the hint and make the moves until there are none left
    while(hint.moves.size() > 0) {
        for (auto &move : hint.moves) {
            move(&board);
        }
        hint = solveHuman(board);
    }

    return board.isWon();
}

void getSolvedPercentage(int count) {
    unsigned int solved_count = 0;
    for (auto i = 0; i < count; i++) {
        std::cout << "Generating board " << i + 1 << '\n';
        auto start_gen = std::chrono::system_clock::now();
        Play board({}, Sudoku::generate(), NULL);
        auto end_gen = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_gen = end_gen - start_gen;
        std::cout << "--> Took: " << elapsed_gen.count() << "s\n";
        auto start_solve = std::chrono::system_clock::now();
        board.autoPencil();
        bool solved = HumanSolveUntilFailed(board);
        auto end_solve = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_solve = end_solve - start_solve;
        if (solved) {
            std::cout << "--> Solved!\n";
            solved_count++;
        }
        else {
            std::cout << "--> Failed\n";
        }
        std::cout << "--> Took: " << elapsed_solve.count() << "s\n";
        float percent = (float)(solved_count) / (i + 1) * 100;
        std::cout << "---> Solved: " << solved_count << '/' << i + 1 << " (" << percent << "%)\n\n";
    }
    std::cout << std::endl;
}