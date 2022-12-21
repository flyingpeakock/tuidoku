#include "HumanSolverPercentage.h"
#include <string>
#include <vector>
#include <algorithm>


int main(int argc, char *argv[]) {
    std::vector<std::string> args;
    for (auto i = 0; i < argc; i++) {
        args.push_back(argv[i]);
    }
    auto find_solve_percent = std::find(args.begin(), args.end(), "human_solve_percent");
    if (find_solve_percent != args.end()) {
        getSolvedPercentage(stoi(*(find_solve_percent + 1)));
    }
}