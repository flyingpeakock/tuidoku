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
static bool makeMove(const Sudoku::logic::LogicalMove &move, std::vector<Sudoku::Move> &moveHistory);

static void unmakeMove(const Sudoku::Move &move);

Sudoku::DancingLinkTable Sudoku::generate(Sudoku::difficulty diff) {
    // TODO: Figure out which to remove first, maybe by looking at which column has the most rows in each step during solve and picking one of those
    bool solve_result = true;
    int current_index = 0;
    DancingLinkTable table = generate();

    difficulty generated_difficulty = eAny;
    while (generated_difficulty != diff) {
        generated_difficulty = grade(table, diff);
        if (generated_difficulty != diff) {
            table = std::move(generate()); // Try again with a new table
        }
    };
    return table;
}

Sudoku::DancingLinkTable Sudoku::generate(std::string string) {
    if (string.size() != eBoardSize) {
        throw std::invalid_argument("Received wrong amount of characters for a sudoku puzzle");
    }
    int row = 0;
    int col = 0;
    DancingLinkTable ret(false);
    for (auto c : string) {
        if (c > '9' || c < '0') {
            throw std::invalid_argument("Received wrong digit for a sudoku puzzle");
        }
        int num = c - '1';
        if (num >= 0) {
            int count = (row * Sudoku::eBoardSize) + (col * Sudoku::eSize) + num;
            int constraints[Sudoku::eConstraintTypes];
            calculateConstraintColumns(constraints, row, col, num);
            auto colHeader = &ret.colHeaders[constraints[0]];
            for (auto row = colHeader->down; row != colHeader; row = row->down) {
                if (row->count == count) {
                    ret.current.push_back(row);
                    break;
                }
            }
        }
        col++;
        if (col == 9) {
            row++;
            col = 0;
        }
    }

    // need to re-cover the links that are in current
    for (auto &link : ret.current) {
        link->colHeader->cover();
        cover_link(link);
    }

    if (!Sudoku::solve(ret, false)) {
        throw std::invalid_argument("Not a valid sudoku puzzle");
    }
    return ret;
}

Sudoku::DancingLinkTable Sudoku::generate(){
    //return Sudoku::generate(eAny);
    bool solve_result = true;
    int current_index = 0;
    DancingLinkTable ret(true);
    DancingLinkContainer solution;

    solve(ret, true); // fills the solution array, all columns are uncovered
    ret.current = ret.solution;
    solution = ret.solution;

    // Shuffle to get a more difficult puzzle
    /*
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(ret.current.begin(), ret.current.end(), g);
    */

    for (auto i = 0; i < eBoardSize; i++) {
        // Storing and removing last
        auto back = ret.current.back(); // never gets covered
        ret.current.pop_back();

        // covering links left in current
        for (auto col = ret.current.begin(); col != ret.current.end(); col++) {
            (*col)->colHeader->cover();
            cover_link(*col);
        }

        bool isUnique = solve(ret, false);

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

static Sudoku::logic::LogicalMove forceMove(Sudoku::DancingLinkTable &table) {
    Sudoku::logic::LogicalMove move;
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

static Sudoku::DancingLink * getTrueLink(Sudoku::DancingLinkTable &table, Sudoku::logic::LogicalMove move) {
    Sudoku::DancingLink *ret;
    switch (move.type) {
        case Sudoku::logic::eLogicInsert:
            ret = move.truths[0];
            break;

        case Sudoku::logic::eLogicPencil:
            for (auto *link : move.truths) {
                auto found = Sudoku::containsLinkEqual(link, table.solution.begin(), table.solution.end());
                if (found  == table.solution.end()) {
                    continue;
                }
                ret = link;
                break;
            }
            break;
        default:
            break;
    }
    return ret;
}

static Sudoku::difficulty grade(Sudoku::DancingLinkTable &table, Sudoku::difficulty requested_difficulty) {
    Sudoku::difficulty highestDifficulty = (Sudoku::difficulty)0;
    std::vector<Sudoku::Move> moveHistory;
    Sudoku::DancingLinkContainer toBeAdded;
    while (table.root.get() != table.root->right) {
        auto move = Sudoku::logic::getNextMove(Sudoku::SudokuPuzzle(table), true);
        if (move.type == Sudoku::logic::eMoveNotFound) {
            move = forceMove(table);
        }

        auto diff = move.diff;
        if (diff > highestDifficulty) {
            highestDifficulty = diff;
        }
        if (diff > requested_difficulty) {
            toBeAdded.emplace_back(getTrueLink(table, move)); // Changing the table, start again
            break;
        }
        makeMove(move, moveHistory);
    }

    // cleaning up table
    for (auto move = moveHistory.rbegin(); move < moveHistory.rend(); move++) {
        unmakeMove(*move);
    }

    if (toBeAdded.size() != 0) { // need to cover some links that were too difficult
        table.current.insert(table.current.end(), toBeAdded.begin(), toBeAdded.end());
        for (const auto &link : toBeAdded) {
            link->colHeader->cover();
            Sudoku::cover_link(link);
        }
        // Need to recalculate since we added some to current
        return grade(table, requested_difficulty);
    }

    return highestDifficulty;
}

static bool makeMove(const Sudoku::logic::LogicalMove &move, std::vector<Sudoku::Move> &moveHistory) {
    bool ret = false;
    switch (move.type) {
        case Sudoku::logic::eLogicPencil: // Remove all pencilmarks that are false
            for (auto *f : move.falses) {
                Sudoku::cover_row(f);
                ret = true;
                moveHistory.emplace_back(Sudoku::eCoverRow, f);
            }
            break;

        case Sudoku::logic::eLogicInsert: // Insert all cells found in true
            for (auto *t : move.truths) {
                t->colHeader->cover();
                Sudoku::cover_link(t);
                ret = true;
                moveHistory.emplace_back(Sudoku::eCoverFull, t);
            }
            break;

        default: // Mistakes aren't handled here
            break;
    }
    return ret;
}

static void unmakeMove(const Sudoku::Move &move) {
    switch (move.type) {
        case Sudoku::eCoverRow:
            Sudoku::uncover_row(move.link);
            break;
        
        case Sudoku::eCoverFull:
            Sudoku::uncover_link(move.link);
            move.link->colHeader->uncover();
            break;

        default:
            break;
    }
}
