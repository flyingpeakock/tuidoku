#pragma once
#include "Sudoku/Sudoku.h"
#include <vector>
#include <iosfwd>

namespace file {
    std::vector<Sudoku::puzzle> getSDKPuzzle(std::istream &file);
    std::vector<Sudoku::puzzle> getXMLPuzzle(std::istream &file);
    std::vector<Sudoku::puzzle> getSDMPuzzle(std::istream &file);
    std::vector<Sudoku::puzzle> getSSPuzzle(std::istream &file);
    std::vector<Sudoku::puzzle> getPuzzle(const char *fileName);
    std::vector<Sudoku::puzzle> getStringPuzzle(const char *puzzleString);
    std::vector<Sudoku::puzzle> getTuidokuPuzzle(std::istream &file);
}