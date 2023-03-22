#pragma once
#include "Sudoku/Sudoku.h"
#include "Play.h"
#include <string>

Hint solveHuman(Play &board);
bool findNakedSingles(Play &board, std::vector<Move> &moves); // beginner
//bool findHiddenSingles(Play &board, const std::uint16_t num, Move *move); // easy
bool findHiddenSingles(Play &board, std::vector<Move> &moves); // easy
bool findPointingBox(Play &board, int i_start, int j_start, std::vector<Move> &moves); // medium
bool findLockedCandidates(Play &board, std::vector<Move> &moves); // medium
bool findHidden(Play &board, const std::uint16_t num, std::vector<Move> &moves); // difficulty depends
bool findNaked(Play &board, const std::uint16_t num, std::vector<Move> &moves); // difficulty depends
bool findBug(Play &board, Move *move); // hard
bool findXwing(Play &board, const std::uint16_t num, std::vector<Move> &moves); // hard or expert
bool findUniqueRectangle(Play &board, const std::uint16_t num, std::vector<Move> &moves); // hard or expert
bool findChainOfPairs(Play &board, const std::uint16_t num, std::vector<Move> &moves); // hard
bool findXYwing(Play &board, std::vector<Move> &moves); // expert
