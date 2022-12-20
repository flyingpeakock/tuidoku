#pragma once
#include "Board.h"
#include "config.h"
#include <string>

struct Move {
    char val;
    char row;
    char col;
    //Board *board;
    bool (Board::*move)(char, int, int);
    bool operator()(Board *board) {
        if (move == NULL) return false;
        //return (board->*move)(val, row, col);
        return (board->*move)(val, row, col);
    }
};

struct Hint {
    std::string hint1;
    std::string hint2;
    std::vector<Move> moves;
};

Hint solveHuman(Board &board);
bool findNakedSingles(Board &board, const std::uint16_t num, Move *move);
bool findHiddenSingles(Board &board, const std::uint16_t num, Move *move);
bool findPointingBox(Board &board, char i_start, char j_start, std::vector<Move> &moves);
bool findLockedCandidates(Board &board, std::vector<Move> &moves);
bool findHidden(Board &board, const std::uint16_t num, std::vector<Move> &moves);
bool findNaked(Board &board, const std::uint16_t num, std::vector<Move> &moves);
bool findBug(Board &board, Move *move);
bool findXwing(Board &board, const std::uint16_t num, std::vector<Move> &moves);
bool findUniqueRectangle(Board &board, const std::uint16_t num, std::vector<Move> &moves);
bool findChainOfPairs(Board &board, const std::uint16_t num, std::vector<Move> &move);
