#pragma once
#include "Sudoku/Sudoku.h"
#include "Play.h"
#include <string>

Hint solveHuman(Sudoku::SudokuObj &board);
bool findNakedSingles(Sudoku::SudokuObj &board, std::vector<Move> &moves); // beginner
//bool findHiddenSingles(Sudoku::SudokuObj &board, const std::uint16_t num, Move *move); // easy
bool findHiddenSingles(Sudoku::SudokuObj &board, std::vector<Move> &moves); // easy
bool findPointingBox(Sudoku::SudokuObj &board, int i_start, int j_start, std::vector<Move> &moves); // medium
bool findLockedCandidates(Sudoku::SudokuObj &board, std::vector<Move> &moves); // medium
bool findHidden(Sudoku::SudokuObj &board, const std::uint16_t num, std::vector<Move> &moves); // difficulty depends
bool findNaked(Sudoku::SudokuObj &board, const std::uint16_t num, std::vector<Move> &moves); // difficulty depends
bool findBug(Sudoku::SudokuObj &board, Move *move); // hard
bool findXwing(Sudoku::SudokuObj &board, const std::uint16_t num, std::vector<Move> &moves); // hard or expert
bool findUniqueRectangle(Sudoku::SudokuObj &board, const std::uint16_t num, std::vector<Move> &moves); // hard or expert
bool findChainOfPairs(Sudoku::SudokuObj &board, const std::uint16_t num, std::vector<Move> &moves); // hard
bool findXYwing(Sudoku::SudokuObj &board, std::vector<Move> &moves); // expert
