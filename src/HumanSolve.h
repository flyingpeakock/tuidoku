#pragma once
#include "Sudoku/Sudoku.h"
#include "Play.h"
#include <string>

Hint solveHuman(Play &board);
bool findNakedSingles(Play &board, const std::uint16_t num, Move *move);
bool findHiddenSingles(Play &board, const std::uint16_t num, Move *move);
bool findPointingBox(Play &board, int i_start, int j_start, std::vector<Move> &moves);
bool findLockedCandidates(Play &board, std::vector<Move> &moves);
bool findHidden(Play &board, const std::uint16_t num, std::vector<Move> &moves);
bool findNaked(Play &board, const std::uint16_t num, std::vector<Move> &moves);
bool findBug(Play &board, Move *move);
bool findXwing(Play &board, const std::uint16_t num, std::vector<Move> &moves);
bool findUniqueRectangle(Play &board, const std::uint16_t num, std::vector<Move> &moves);
bool findChainOfPairs(Play &board, const std::uint16_t num, std::vector<Move> &move);
