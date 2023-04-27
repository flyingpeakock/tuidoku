#include "Sudoku.h"
#include "Logic.h"
#include <vector>
#include <cstring>
#include <algorithm>
#include <random>

/**
 * @brief Determines the difficulty of a puzzle
 * 
 * The difficulty is equal to the difficulty of the hardest move
 * 
 * @param table constraint table containing the puzzle.
 *              The cells which are filled need to be covered
 * @param requested_difficulty the difficulty which has been requested to create. Will fill in cells above this difficulty
 * @return Sudoku::difficulty 
 */
static Sudoku::difficulty grade(Sudoku::DancingLinkTable &table, Sudoku::difficulty requested_difficulty);

/**
 * @brief makes a move found using logic
 * 
 * @param move move found
 */
static void makeMove(const Sudoku::logic::Move &move);

Sudoku::DancingLinkTable Sudoku::generate(Sudoku::difficulty diff) {
    // TODO: Figure out which to remove first, maybe by looking at which column has the most rows in each step during solve and picking one of those
    bool solve_result = true;
    int current_index = 0;
    DancingLinkTable table = generate();

    difficulty generated_difficulty = eAny;
    while (generated_difficulty != diff) {
        generated_difficulty = grade(table, diff);
        if (generated_difficulty != diff) {
            table = generate(); // Try again with a new table
        }
    }
    return table;
}

Sudoku::DancingLinkTable Sudoku::generate(){
    //return Sudoku::generate(eAny);
    bool solve_result = true;
    int current_index = 0;
    DancingLinkTable ret(true);
    std::vector<DancingLink *> solution;

    solve(&ret, true); // fills the solution array, all columns are uncovered
    ret.current = ret.solution;
    solution = ret.solution;

    // Shuffle to get a more difficult puzzle
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(ret.current.begin(), ret.current.end(), g);

    for (auto i = 0; i < eBoardSize; i++) {
        // Storing and removing last
        auto back = ret.current.back(); // never gets covered
        ret.current.pop_back();

        // covering links left in current
        for (auto col = ret.current.begin(); col != ret.current.end(); col++) {
            (*col)->colHeader->cover();
            cover_link(*col);
        }

        bool isUnique = solve(&ret, false);

        // uncovering links in opposite order
        for (auto col = ret.current.rbegin(); col != ret.current.rend(); col++) {
            uncover_link(*col);
            (*col)->colHeader->uncover();
        }
        
        if (!isUnique) {
            // move to front so we don't try it again
            ret.current.insert(ret.current.begin(), back);
        }
        // All links are uncovered again
    }

    // need to re-cover the links that are in current
    for (auto &link : ret.current) {
        link->colHeader->cover();
        cover_link(link);
    }
    ret.solution = solution;
    
    return ret;
} 

static bool madeMoveInVector(const std::vector<bool (*)(Sudoku::DancingLink *, Sudoku::logic::Move &)> &vector,
                             Sudoku::DancingLink *root,
                             Sudoku::logic::Move &move) {
    bool ret = false;
    for (auto func : vector) {
        ret = func(root, move);
        if (ret) {
            makeMove(move);
            return ret;
        }
    }
    return ret;
}

static bool madeMoveInVectors(const std::vector<std::vector<bool (*)(Sudoku::DancingLink *, Sudoku::logic::Move &)>> &vectors,
                              Sudoku::DancingLink *root, Sudoku::logic::Move &move) {
    for (auto vec : vectors) {
        if (madeMoveInVector(vec, root, move)) {
            return true;
        }
    }
    return false;
}

static Sudoku::logic::Move forceMove(Sudoku::DancingLinkTable &table) {
    Sudoku::logic::Move move;
    auto colToCover = Sudoku::smallestColumn(table.root.get(), true);
    for (auto row = colToCover->down; row != colToCover; row = row->down) {
        auto found = Sudoku::containsLinkEqual(row, table.solution.begin(), table.solution.end());
        if (found == table.solution.end()) {
            continue;
        }

        move.diff = (colToCover->count * 2) > (Sudoku::eDifficulties - 1) ? (Sudoku::difficulty)(Sudoku::eDifficulties - 1) : (Sudoku::difficulty)(colToCover->count * 2);
        move.type = Sudoku::logic::eLogicInsert;
        move.truths.push_back(row);
        break;
    }
    return move;
}

static void addMoveToCurrent(Sudoku::DancingLinkTable &table, Sudoku::logic::Move move) {
    switch (move.type) {
        case Sudoku::logic::eLogicInsert:
            table.current.push_back(move.truths[0]);
            break;

        case Sudoku::logic::eLogicPencil:
            for (auto *link : move.truths) {
                auto found = Sudoku::containsLinkEqual(link, table.solution.begin(), table.solution.end());
                if (found  == table.solution.end()) {
                    continue;
                }
                table.current.push_back(link);
                break;
            }
            break;
        default:
            break;
    }
}

static Sudoku::difficulty grade(Sudoku::DancingLinkTable &table, Sudoku::difficulty requested_difficulty) {
    Sudoku::difficulty highestDifficulty = (Sudoku::difficulty)0;
    while (table.root.get() != table.root->right) {
        auto moves = Sudoku::logic::getNextMove(Sudoku::SudokuPuzzle(&table), true);
        if (moves.size() == 0) {
            moves.push_back(forceMove(table));
        }

        auto diff = moves[0].diff;
        if (diff > highestDifficulty) {
            highestDifficulty = diff;
        }
        if (diff > requested_difficulty) {
            addMoveToCurrent(table, moves[0]); // Changing the table, start again
            break;
        }
        for (auto &move : moves) {
            if (move.diff > diff) { // Only make moves of the same difficulty
                break;
            }
            makeMove(move);
        }
    }

    // cleaning up table
    table.generateLinks(true);
    for (auto &link : table.current) {
        link->colHeader->cover();
        Sudoku::cover_link(link);
    }

    // Need to recalculate since we added some to current
    if (highestDifficulty > requested_difficulty) {
        return grade(table, requested_difficulty);
    }
    return highestDifficulty;

/*
    if (highestDifficulty <= requested_difficulty) {
        // Did not change anything
        return highestDifficulty;
    }

    return grade(table, requested_difficulty);
*/
}

static void makeMove(const Sudoku::logic::Move &move) {
    switch (move.type) {
        case Sudoku::logic::eLogicPencil: // Remove all pencilmarks that are false
            for (auto f : move.falses) {
                Sudoku::cover_row(f);
            }
            break;

        case Sudoku::logic::eLogicInsert: // Insert all cells found in true
            for (auto t : move.truths) {
                t->colHeader->cover();
                Sudoku::cover_link(t);
            }
            break;

        default: // Mistakes aren't handled here
            break;
    }
}
