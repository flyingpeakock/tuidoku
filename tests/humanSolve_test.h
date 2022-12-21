#pragma once

#include "../src/HumanSolve.h"
#include "../src/Generator.h"

struct single_digit_human_solve_t {
    std::uint16_t num;
    Move move;
    int moves;
    bool ret;
    const char *gridString;
};
