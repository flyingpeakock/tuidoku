#include "Sudoku.h"

using namespace Sudoku;

int Sudoku::getRowFromLink(DancingLink *link) {
    return link->count / eBoardSize;
}

int Sudoku::getColFromLink(DancingLink *link) {
    return (link->count % eBoardSize) / eSize;
}

int Sudoku::getNumFromLink(DancingLink *link) {
    return (link->count % eSize) + 1;
}