#include "HumanSolve.h"
#include <map>
#include <unordered_set>
#include <utility>
#include <algorithm>
#include <sstream>

struct Coord {
    char i;
    char j;
};

static std::string getGenericHint(std::vector<Move> moves) {
    std::stringstream ret;
    ret << "Look closer at the digit ";
    ret << moves[0].val;
    std::uint16_t seen = (1 << (moves[0].val - START_CHAR));
    for (size_t i = 1; i < moves.size(); i++) {
        if ((seen & (1 << (moves[i].val - START_CHAR))) != 0) continue;
            ret << " and " << moves[i].val;
            seen |= (1 << (moves[i].val - START_CHAR));
    }
    return ret.str();
}

static void getCombinations(int offset, int k, std::uint16_t combinations, std::vector<std::uint16_t> &final) {
    if (k == 0) {
        final.emplace_back(combinations);
        return;
    }

    for (int i = offset; i < 9; i++) {
        combinations |= (1 << i);
        getCombinations(i + 1, k - 1, combinations, final);
        combinations &= ~(1 << i);
    }
}

static std::vector<uint16_t> getCombinations(int k) {
    std::vector<uint16_t> final;
    getCombinations(0, k, 0, final);
    return final;
}

static int countBits(std::uint16_t bits) {
    int count = 0;
    while (bits != 0) {
        if ((bits & 1) == 1) {
            count++;
        }
        bits >>=1;
    }
    return count;
}

 static std::vector<char> getSetBits(std::uint16_t bits) {
    std::vector<char> nums;
    for (unsigned char num = 0; num < 9; num++) {
        if ((bits & (1 << num)) != 0) {
            nums.push_back(num);
        }
    }
    return nums;
 }

static char countOccurrencesNaked(Board &board, std::uint16_t bits, char i_min, char i_max, char j_min, char j_max, std::uint16_t &seen_i, std::uint16_t &seen_j) {
    char count = 0;
    for (auto i = i_min; i < i_max; i++) {
        for (auto j = j_min; j < j_max; j++) {
            if (!board.isEmpty(i, j)) continue;
            std::uint16_t marks = board.getPencil(i, j);
            if ((marks & bits) == 0) continue; // does not exist here
            if ((marks | bits) == bits){ // only exists here
                seen_i |= (1 << i);
                seen_j |= (1 << j);
                count++;
            }
        }
    }
    return count;
}

static char countOccurrencesHidden(Board &board, std::uint16_t bits, char i_min, char i_max, char j_min, char j_max, std::uint16_t &seen_i, std::uint16_t &seen_j) {
   char count = 0;
   std::uint16_t seen_vals = 0;
   for (auto i = i_min; i < i_max; i++) {
        for (auto j = j_min; j < j_max; j++) {
            if (!board.isEmpty(i, j)) continue;
            auto marks = board.getPencil(i ,j);
            if ((marks & bits) != 0) {
                seen_i |= (1 << i);
                seen_j |= (1 << j);
                seen_vals |= (marks & bits);
                count++;
            }
        }
    }
    if (seen_vals != bits) return 0xFF;
    return count;
}

static bool removedOccurrencesNaked(Board &board, std::uint16_t bits, char i_min, char i_max, char j_min, char j_max, std::vector<Move> &moves) {
    bool ret = false;
    for (auto i = i_min; i < i_max; i++) {
        for (auto j = j_min; j < j_max; j++) {
            if (!board.isEmpty(i, j)) continue;
            std::uint16_t marks = board.getPencil(i, j);
            if ((marks & ~(bits)) == 0) continue; // Only marks are set here
            marks &= bits; // marks == bits that should be removed
            if ((marks) == 0) continue; // no marks set here
            auto set_bits = getSetBits(marks);
            for (auto &num : set_bits) {
                //board.pencil(num + START_CHAR, i, j);
                Move move = {
                    (char)(num + START_CHAR), i, j, &Board::pencil
                };
                moves.push_back(move);
                ret = true;
            }
        }
    }
    return ret;
}

static bool removedOccurrencesHidden(Board &board, std::uint16_t bits, char i_min, char i_max, char j_min, char j_max, std::vector<Move> &moves) {
    bool ret = false;
    for (auto i = i_min; i < i_max; i++) {
        for (auto j = j_min; j < j_max; j++) {
            if (!board.isEmpty(i, j)) continue;
            std::uint16_t marks = board.getPencil(i, j);
            if ((marks & bits) == 0) continue;
            marks &= ~bits;
            auto set_bits = getSetBits(marks);
            for (auto &num : set_bits) {
                //board->pencil(num + START_CHAR, i, j);
                Move move = {
                    (char)(num + START_CHAR), i, j, &Board::pencil
                };
                moves.push_back(move);
                ret = true;
            }
        }
    }
    return ret;
}


Hint solveHuman(Board &board) {
    Hint hint = {
        "",
        "",
        {},
    };

    if (board.isWon()){
        hint.hint1 = "Board is solved!";
        hint.hint2 = "Board is solved!";
        return hint;
    }

    static auto all_singles = getCombinations(1);
    static auto all_doubles = getCombinations(2);
    static auto all_triples = getCombinations(3);
    static auto all_quads = getCombinations(4);
    for (auto &num : all_singles) {
        Move single_move;
        if (findNakedSingles(board, num, &single_move)) {
            //return true;
            hint.moves.push_back(single_move);
            std::stringstream hint_1_stream;
            std::stringstream hint_2_stream;
            hint_1_stream << "Look closer at the digit " << hint.moves[0].val;
            hint_2_stream << "row " << (char)(hint.moves[0].row + START_CHAR) << " column " << (char)(hint.moves[0].col + START_CHAR) << " can only be " << hint.moves[0].val;
            hint.hint1 = hint_1_stream.str();
            hint.hint2 = hint_2_stream.str();
            return hint;

        }
        if (findHiddenSingles(board, num, &single_move)) {
            hint.moves.push_back(single_move);
            std::stringstream hint_1_stream;
            std::stringstream hint_2_stream;
            hint_1_stream << "Look closer at the digit " << hint.moves[0].val;
            hint_2_stream << "row " << (char)(hint.moves[0].row + START_CHAR) << " column " << (char)(hint.moves[0].col + START_CHAR) << " is the only possible location for " << hint.moves[0].val;
            hint.hint1 = hint_1_stream.str();
            hint.hint2 = hint_2_stream.str();
            return hint;
        }
    }
    
    for (char i = 0; i < 9; i += 3) {
        for (char j = 0; j < 9; j += 3) {
            if (findPointingBox(board, i, j, hint.moves)) {
                std::stringstream hint_1_stream;
                std::stringstream hint_2_stream;
                hint_1_stream << "Look closer at the digit " << hint.moves[0].val;
                hint_2_stream << "The digit " << hint.moves[0].val << " forms a pointing box.";
                hint.hint1 = hint_1_stream.str();
                hint.hint2 = hint_2_stream.str();
                return hint;
            }
        }
    }

    for (auto num : all_doubles) {
        if (findNaked(board, num, hint.moves)) {
            hint.hint1 = "Look for a naked double";
            hint.hint2 = getGenericHint(hint.moves);
            return hint;
        }
        if (findHidden(board, num, hint.moves)) {
            hint.hint1 = "Look for a hidden double";
            hint.hint2 = getGenericHint(hint.moves);
            return hint;
        }
    }

    if (findLockedCandidates(board, hint.moves)) {
        std::stringstream hint_1_stream;
        std::stringstream hint_2_stream;
        hint.hint1 = "Look for locked candidates";
        hint.hint2 = getGenericHint(hint.moves);
        return hint;
    }
    for (auto &triple : all_triples) {
        if (findNaked(board, triple, hint.moves)) {
            //return true;
            hint.hint1 = "Look for a naked triple";
            hint.hint2 = getGenericHint(hint.moves);
            return hint;
        }
    }
    for (auto &quad : all_quads) {
        if (findNaked(board, quad, hint.moves)) {
            //return true;
            hint.hint1 = "Look for a naked quadtruple";
            hint.hint2 = getGenericHint(hint.moves);
            return hint;
        }
    }
    for (auto &triple : all_triples) {
        if (findHidden(board, triple, hint.moves)) {
            //return true;
            hint.hint1 = "Look for a hidden triple";
            hint.hint2 = getGenericHint(hint.moves);
            return hint;
        }
    }
    for (auto &quad : all_quads) {
        if (findHidden(board, quad, hint.moves)) {
            //return true;
            hint.hint1 = "Look for a hidden quadruple";
            hint.hint2 = getGenericHint(hint.moves);
            return hint;
        }
    }
    Move singleMove;
    if (findBug(board, &singleMove)) {
        hint.moves.push_back(singleMove);
        hint.hint1 = "Look for a bug";
        std::stringstream hintStream;
        hintStream << "The digit " << hint.moves[0].val << "can only go in row " << (char)(hint.moves[0].row + START_CHAR) << "and in column " << (char)(hint.moves[0].col + START_CHAR);
        return hint;
    }
    for (auto &doub : all_doubles) {
        if (findChainOfPairs(board, doub, hint.moves)) {
            hint.hint1 = "Look for a chain of pairs";
            hint.hint2 = getGenericHint(hint.moves);
            return hint;
        }
    }
    for (auto &doub : all_doubles) {
        if (findUniqueRectangle(board, doub, hint.moves)) {
            //return true;
            hint.hint1 = "Look for a unique rectangle";
            hint.hint2 = getGenericHint(hint.moves);
            return hint;
        }
    }
    for (auto &single : all_singles) {
        if (findXwing(board, single, hint.moves)) {
            //return true;
            hint.hint1 = "Look for an X-wing";
            std::stringstream hintstr;
            hintstr << "Look closer at the digit ";
            hintstr << hint.moves[0].val;
            hint.hint2 = hintstr.str();
            return hint;
        }
    }

    hint.hint1 = "Unable to give any hints";
    hint.hint2 = "This is out of my league";
    return hint;
}

/**
 * @brief finds any naked singles in the board
 * 
 * @param board that should be searched through
 * @param single number with bits set that represent the digit being searched for
 * @param move pointer to Move that holds information about the next move
 * @return true when found a naked single
 * @return false otherwise
 */
bool findNakedSingles(Board &board, const std::uint16_t single, Move *move) {
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            if (!board.isEmpty(i, j)) continue;
            if (board.getPencil(i, j) == single) {
                //board->insert(getSetBits(single)[0] + START_CHAR, i ,j);
                (*move).col = j;
                (*move).row = i;
                (*move).val = getSetBits(single)[0] + START_CHAR;
                (*move).move = &SimpleBoard::insert;
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Finds hidden singles
 * 
 * @param board to search through
 * @param single number to search for where the number is the position of the set bit
 * @param move pointer to move which stores information about the next move
 * @return true when a hidden single is found
 * @return false otherwise
 */
bool findHiddenSingles(Board &board, const std::uint16_t single, Move *move) {
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            if (!board.isEmpty(i ,j)) continue;
            if ((board.getPencil(i, j) & single) == 0) continue;
            auto i_box = (i / 3) * 3;
            auto j_box = (j / 3) * 3;
            std::uint16_t trash;
            if ((countOccurrencesHidden(board, single, i_box, i_box + 3, j_box, j_box + 3, trash, trash) == 1)
                || (countOccurrencesHidden(board, single, i, i + 1, 0, 9, trash, trash) == 1)
                || (countOccurrencesHidden(board, single, 0, 9, j, j + 1, trash, trash) == 1)) {
                (*move).col = j;
                (*move).row = i;
                (*move).val = getSetBits(single)[0] + START_CHAR;
                (*move).move = &SimpleBoard::insert;
                return true;
            }
        }
    }
    return false;
}

bool findNaked(Board &board, const std::uint16_t num, std::vector<Move> &moves) {
    std::uint16_t trash;
    if (num == 0) return false;
    const char matcher = countBits(num);
    // finding in box
    for (auto i_box = 0; i_box < 9; i_box += 3) {
        for (auto j_box = 0; j_box < 9; j_box += 3) {
            auto count = countOccurrencesNaked(board, num, i_box, i_box + 3, j_box, j_box + 3, trash, trash);
            if (count != matcher) continue;
            if (removedOccurrencesNaked(board, num, i_box, i_box + 3, j_box, j_box + 3, moves)) {
                return true;
            }
        }
    }

    // rows and cols
    for (auto i = 0; i < 9; i++) {
        auto count =  countOccurrencesNaked(board, num, i, i+1, 0, 9, trash, trash);
        if (count == matcher) {
            if (removedOccurrencesNaked(board, num, i, i+1, 0, 9, moves)) {
                return true;
            }
        }
        count = countOccurrencesNaked(board, num, 0, 9, i, i+1, trash, trash);
        if (count == matcher) {
            if (removedOccurrencesNaked(board, num, 0, 9, i, i+1, moves)) {
                return true;
            }
        }
    }
    return false;
}


bool findHidden(Board &board, const std::uint16_t num, std::vector<Move> &moves) {
    if (num == 0) return false;
    const char matcher = countBits(num);
    std::uint16_t trash;
    // finding in box
    for (auto i_box = 0; i_box < 9; i_box += 3) {
        for (auto j_box = 0; j_box < 9; j_box += 3) {
            char count = countOccurrencesHidden(board, num, i_box, i_box + 3, j_box, j_box + 3, trash, trash);
            if (count != matcher) continue;
            if (removedOccurrencesHidden(board, num, i_box, i_box + 3, j_box, j_box + 3, moves)) {
                return true;
            }
        }
    }

    // finding in rows and cols
    for (auto i = 0; i < 9; i++) {
        char count = countOccurrencesHidden(board, num, i, i+1, 0, 9, trash, trash);
        if (count == matcher) {
            if (removedOccurrencesHidden(board, num, i, i+1, 0, 9, moves)) {
                return true;
            }
        }
        count = countOccurrencesHidden(board, num, 0, 9, i, i+1, trash, trash);
        if (count == matcher) {
            if (removedOccurrencesHidden(board, num, 0, 9, i, i+1, moves)) {
                return true;
            }
        }
    }
    return false;
}

static bool removeMarks_i_box(Board &board, char val, char i, char j_box, std::vector<Move> &moves) {
    auto pencil = board.getPencilMarks();
    for (char idx = 0; idx < 9; idx++) {
        if (((idx / 3) * 3) == j_box) continue; // same box as pointers
        if (!board.isEmpty(i, idx)) continue;
        if ((pencil[i][idx] & (1 << val)) != 0) {
            //board->pencil(val + START_CHAR, i, idx);
            Move move = {(char)(val + START_CHAR), i, idx, &Board::pencil};
            moves.push_back(move);
            return true;
        }
    }
    return false;
}

static bool removeMarks_j_box(Board &board, char val, char j, char i_box, std::vector<Move> &moves) {
    auto pencil = board.getPencilMarks();
    for (char idx = 0; idx < 9; idx++) {
        if (((idx / 3) * 3) == i_box) continue;
        if (!board.isEmpty(idx, j)) continue;
        if ((pencil[idx][j] & (1 << val)) != 0) {
            //board->pencil(val + START_CHAR, idx, j);
            Move move = {(char)(val + START_CHAR), idx, j, &Board::pencil};
            moves.push_back(move);
            return true;
        }
    }
    return false;
}

bool findPointingBox(Board &board, char i_start, char j_start, std::vector<Move> &moves) {
    char counts[9] = {};
    bool indexes[9][3][3] = {false};
    auto marks = board.getPencilMarks();
    for (auto i = i_start; i < i_start + 3; i++) {
        for (auto j = j_start; j < j_start + 3; j++) {
            if (!board.isEmpty(i, j)) continue;
            for (unsigned char num = 0; num < 9; num++) {
                if ((marks[i][j] & (1 << num)) == 0) continue;
                counts[num]++;
                indexes[num][i - i_start][j - j_start] = true;
            }
        }
    }
    for (unsigned char num = 0; num < 9; num++) {
        if (counts[num] > 3) continue;

        for (auto i = 0; i < 3; i++) {
            if (indexes[num][i][0] || indexes[num][i][1] || indexes[num][i][2]) {
                // exists in this index
                bool should_break = false;
                for (auto j = 1; j <= 2; j++) {
                    if (indexes[num][(i + j) % 3][0] || indexes[num][(i + j) % 3][1] || indexes[num][(i + j) % 3][2]) {
                        // also exists in this index
                        should_break = true;
                    }
                }
                if (should_break) break;
                if (removeMarks_i_box(board, num, i + i_start, j_start, moves)) {
                    return true;
                }
            }
        }
        for (auto j = 0; j < 3; j++) {
            if (indexes[num][0][j] || indexes[num][1][j] || indexes[num][2][j]) {
                // exists in this index
                bool should_break = false;
                for (auto i = 1; i <= 2; i++) {
                    if (indexes[num][0][(j + i) % 3] || indexes[num][1][(j + i) % 3] || indexes[num][2][(j + i) % 3]) {
                        // also in this index
                        should_break = true;
                    }
                }
                if (should_break) break;
                if (removeMarks_j_box(board, num, j + j_start, i_start, moves)) {
                    return true;
                };
            }
        }
    }
    return false;
}

static bool existsOnlyInBox(Board &board, char box_i, char box_j, char i_min, char i_max, char j_min, char j_max, char num) {
    auto pencils = board.getPencilMarks();
    for (auto i = i_min; i < i_max; i++) {
        for (auto j = j_min; j < j_max; j++) {
            if (!board.isEmpty(i, j)) continue;
            if (((i / 3) * 3) == box_i && ((j / 3) * 3) == box_j) continue;
            if ((pencils[i][j] & (1 << num)) != 0) {
                return false;
            }
        }
    }
    return true;
}

static bool removedLockedIFromBox(Board &board, char box_i, char box_j, char locked_i, char num, std::vector<Move> &moves) {
    auto pencils = board.getPencilMarks();
    bool ret = false;
    for (auto i = box_i; i < box_i + 3; i++) {
        for (auto j = box_j; j < box_j + 3; j++) {
            if (!board.isEmpty(i, j)) continue;
            if (i == locked_i) continue;
            if ((pencils[i][j] & (1 << num)) != 0) {
                //board->pencil(num + START_CHAR, i, j);
                Move move = {(char)(num + START_CHAR), i, j, &Board::pencil};
                moves.push_back(move);
                ret = true;
            }
        }
    }
    return ret;
}

static bool removedLockedJFromBox(Board &board, char box_i, char box_j, char locked_j, char num, std::vector<Move> &moves) {
    auto pencils = board.getPencilMarks();
    bool ret = false;
    for (auto i = box_i; i < box_i + 3; i++) {
        for (auto j = box_j; j < box_j + 3; j++) {
            if (!board.isEmpty(i, j)) continue;
            if (j == locked_j) continue;
            if ((pencils[i][j] & (1 << num)) != 0) {
                //board->pencil(num + START_CHAR, i, j);
                Move move = {(char)(num + START_CHAR), i, j, &Board::pencil};
                moves.push_back(move);
                ret = true;
            }
        }
    }
    return ret;
}

bool findLockedCandidates(Board &board, std::vector<Move> &moves) {
    auto pencils = board.getPencilMarks();
    for (auto box_i = 0; box_i < 9; box_i += 3) {
        for (auto box_j = 0; box_j < 9; box_j += 3) {
            for (auto i = box_i; i < box_i + 3; i++) {
                for (auto j = box_j; j < box_j + 3; j++) {
                    if (!board.isEmpty(i, j)) continue;
                    for (unsigned char num = 0; num < 9; num++) {
                        if ((pencils[i][j] & (1 << num)) == 0) continue;
                        if (existsOnlyInBox(board, box_i, box_j, i, i+1, 0, 9, num)) {
                            if (removedLockedIFromBox(board, box_i, box_j, i, num, moves)) {
                                return true;
                            }
                        }
                        if (existsOnlyInBox(board, box_i, box_j, 0, 9, j, j+1, num)) {
                            if (removedLockedJFromBox(board, box_i, box_j, j, num, moves)) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool findBug(Board &board, Move *move) {
    int numb_of_threes = 0;
    int col = 0xFF;
    int row = 0xFF;
    std::uint16_t bits = 0;
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            if (!board.isEmpty(i, j))continue;
            auto pencils = board.getPencil(i, j);
            auto count = countBits(pencils);
            if (count > 3) return false;
            if (count == 3) {
                numb_of_threes++;
                col = i;
                row = j;
                bits = pencils;
            }
        }
    }
    if (numb_of_threes != 1) return false;

    auto numbs = getSetBits(bits);
    int col_box = (col / 3) * 3;
    int row_box = (row / 3) * 3;
    for (auto num: numbs) {
        std::uint16_t num_bits = (1 << num);
        std::uint16_t trash;
        if ((countOccurrencesHidden(board, num_bits, col, col + 1, 0, 9, trash, trash) == 3)
          && (countOccurrencesHidden(board, num_bits, 0, 9, row, row + 1, trash, trash) == 3)
          && (countOccurrencesHidden(board, num_bits, col_box, col_box + 3, row_box, row_box + 3, trash, trash)) == 3) {
            //board->insert(num + START_CHAR, col, row);
            (*move).col = row;
            (*move).row = col;
            (*move).val = num + START_CHAR;
            (*move).move = &SimpleBoard::insert;
            return true;
        }
    }
    return false;
}

/**
 * @brief Gets the indexes of the positions in the array matching curr
 * 
 * The matching indexes bits are set in the return value
 * 
 * @param positions array containing the positions of the value being searched for in row/box
 * @param curr the current indexes to match with
 * @return the indexes in positions that match with curr as set bits in a 16bit integer 
 */
static std::uint16_t getEqualWingIndexes(std::array<std::uint16_t, 9> positions, std::uint16_t curr) {
    std::uint16_t indexes = 0;
    int index = 0;
    for (auto &pos : positions) {
        if (pos == curr) {
            indexes |= (1 << index);
        }
        index++;
    }
    return indexes;
}

/**
 * @brief remove pencils marks found by X-wing in rows
 * 
 * This function gets called if there are matching X-wings found in rows
 * 
 * @param board that the X-wing was found in
 * @param num that matches the X-wing. Num is the set bit
 * @param i_indexes Row indexes that the X-wing was found on
 * @param j_indexes Column indexes that the X-wing was found on
 * @param moves Vector that gets populated with the correct moves
 * @return true when correct moves have been found
 * @return false otherwise
 */
static bool removeXwingByRows(Board &board, const std::uint16_t num, std::uint16_t i_indexes, std::uint16_t j_indexes, std::vector<Move> &moves) {
    bool ret = false;
    for (char i = 0; i < 9; i++) {
        if ((i_indexes & (1 << i)) != 0) continue;
        for (char j : getSetBits(j_indexes)) {
            if (!board.isEmpty(i, j)) continue;
            auto marks = board.getPencil(i, j) & num;
            if (marks == 0) continue;
            for (auto unset : getSetBits(marks)) {
                Move move = {(char)(unset + START_CHAR), i, j, &Board::pencil};
                moves.push_back(move);
                ret = true;
            }
        }
    }
    return ret;
}

/**
 * @brief Adds moves to the vector that are found by the X-Wing
 * 
 * This function gets called if there are matching x-wings found in columns.
 * 
 * @param board that the X-wing has been found in
 * @param num that matches the X-wing. Num is the set bit
 * @param i_indexes row indexes where the x-wing is located
 * @param j_indexes col indexes where the x-wing is located
 * @param moves vector that found moves gets put into
 * @return true if found any pencil marks to remove
 * @return false otherwise
 */
static bool removeXwingByCols(Board &board, const std::uint16_t num, std::uint16_t i_indexes, std::uint16_t j_indexes, std::vector<Move> &moves) {
    bool ret = false;
    for (char i : getSetBits(i_indexes)) {
        for (char j = 0; j < 9; j++) {
            if ((j_indexes & (1 << j)) != 0) continue; // same position as x-wing
            if (!board.isEmpty(i, j)) continue;
            auto marks = board.getPencil(i, j) & num;
            if (marks == 0) continue;
            for (auto unset : getSetBits(marks)) {
                Move move = {(char)(unset + START_CHAR), i, j, &Board::pencil};
                moves.push_back(move);
                ret = true;
            }
        }
    }
    return ret;
}

/**
 * @brief Looks for xwings in the board
 * 
 * @param board to look through
 * @param num to match with the x-wing
 * @param moves vector that gets filled with the correct moves, if any
 * @return true  if found any x-wings
 * @return false otherwise
 */
bool findXwing(Board &board, const std::uint16_t num, std::vector<Move> &moves) {
    std::array<std::uint16_t, 9> positions[2];
    for (auto i = 0; i < 9; i++) {
        std::uint16_t seen_j, seen_i, trash;
        seen_j = seen_i = 0;
        countOccurrencesHidden(board, num, i, i+1, 0, 9, trash, seen_j);
        positions[0][i] = seen_j;

        countOccurrencesHidden(board, num, 0, 9, i, i+1, seen_i, trash);
        positions[1][i] = seen_i;
    }
    for (auto i = 0; i < 9; i++) {
        // indexes are columns where we should remove
        std::uint16_t indexes = getEqualWingIndexes(positions[0], positions[0][i]);
        if (countBits(indexes) != countBits(positions[0][i])) continue;
        if (removeXwingByRows(board, num, indexes, positions[0][i], moves)) {
            return true;
        }
    }
    for (auto i = 0; i < 9; i++) {
        std::uint16_t indexes = getEqualWingIndexes(positions[1], positions[1][i]);
        if (countBits(indexes) != countBits(positions[1][i])) continue;
        if (removeXwingByCols(board, num, positions[1][i], indexes, moves)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Looks for unique rectangles
 * 
 * @param board to search through
 * @param num 2 digits that form the rectangle
 * @param moves vector that gets populated with found moves
 * @return true if pencil marks can be removed
 * @return false otherwise
 */
bool findUniqueRectangle(Board &board, const std::uint16_t num, std::vector<Move> &moves) {
    if (num == 0) return false;
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            std::uint16_t i_pos, j_pos, trash;
            i_pos = j_pos = 0;
            if (!board.isEmpty(i ,j)) continue;
            if ((board.getPencil(i, j)) != num) continue;
            if (countOccurrencesNaked(board, num, i, i+1, 0, 9, trash, j_pos) != 2) continue;
            if (countOccurrencesNaked(board, num, 0, 9, j, j+1, i_pos, trash) != 2) continue;
            i_pos &= ~(1 << i); // remove the location that we are at
            j_pos &= ~(1 << j); // the one remaining is the intersection
            if ((i_pos == 0) || j_pos == 0) continue;
            auto intersect_i = getSetBits(i_pos)[0];
            auto intersect_j = getSetBits(j_pos)[0];
            // current location has box start idx i/3*3, j/3*3
            // one intersection has i/3*3, j_pos/3*3
            // other one has i_pos/3*3/ j/3*3
            char boxi1, boxj1, boxi2, boxj2, boxi3, boxj3;
            boxi1 = (i / 3) * 3;
            boxj1 = (j / 3) * 3;
            boxi2 = (i / 3) * 3;
            boxj2 = (j_pos / 3) * 3;
            boxi3 = (i_pos / 3) * 3;
            boxj3 = (j / 3) * 3;
            if (!((boxi1 == boxi2 && boxj1 == boxj2) || (boxi1 == boxi3 && boxj1 == boxj3) || (boxi2 == boxj3 && boxj2 == boxj3))) continue;
            if (!board.isEmpty(intersect_i, intersect_j)) continue;
            auto mark = board.getPencil(intersect_i, intersect_j) & num;
            if (mark == 0) continue;
            for (auto &unset : getSetBits(mark)) {
                Move move = { (char)(unset + START_CHAR), intersect_i, intersect_j, &Board::pencil };
                moves.push_back(move);
            }
            return true;
        }
    }
    return false;
}

/**
 * @brief checks if two boxes are in the same unit
 * 
 * @param i_1 first box row
 * @param j_1 first box column
 * @param i_2 second box row
 * @param j_2 second box column
 * @return true if they are in the same unit
 * @return false if they aren't in the same unit
 */
static bool canSee(char i_1, char j_1, char i_2, char j_2) {
    if (i_1 == i_2) return true;
    if (j_1 == j_2) return true;
    char box_i_1, box_i_2, box_j_1, box_j_2;
    box_i_1 = (i_1 / 3) * 3;
    box_i_2 = (i_2 / 3) * 3;
    box_j_1 = (j_1 / 3) * 3;
    box_j_2 = (j_2 / 3) * 3;
    return ((box_i_1 == box_i_2) && (box_j_1 == box_j_2));
}

/**
 * @brief seperates the all vector into two chains that see each other
 * 
 * @param chain_a starts with one item in it
 * @param chain_b empty vector
 * @param all all the found doubles
 * @return true if a link has been added to the chain
 * @return false all links have been searched through
 */
static bool build_chain(std::vector<Coord> &chain_a, std::vector<Coord> &chain_b,  std::vector<Coord> &all) {
    if (all.size() == 0) return false;
    for (std::vector<Coord>::iterator link_itr = all.begin(); link_itr != all.end(); link_itr++) {
        for (auto a : chain_a) {
            if (canSee((*link_itr).i, (*link_itr).j, a.i, a.j)) {
                chain_b.push_back(*link_itr);
                link_itr = all.erase(link_itr);
                return true;
            }
        }
        for (auto b : chain_b) {
            if (canSee((*link_itr).i, (*link_itr).j, b.i, b.j)) {
                chain_a.push_back(*link_itr);
                link_itr = all.erase(link_itr);
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief looks for values that are seen by both chains and removes the pencil mark
 * 
 * @param board board that the chains have been found in
 * @param num pair that forms the chain
 * @param chain_a positions that see chain_b
 * @param chain_b positions that see chain_a
 * @param moves vector that found moves gets put into
 * @return true if moves are found
 * @return false if moves are not found
 */
static bool removedByChain(Board &board, const std::uint16_t num, std::vector<Coord> chain_a, std::vector<Coord> chain_b, std::vector<Move> &moves) {
    for (char i = 0; i < 9; i++) {
        for (char j = 0; j < 9; j++) {
            if (!board.isEmpty(i, j)) continue;
            auto marks = board.getPencil(i, j) & num;
            if (marks == 0) continue;
            if (marks == num) continue;
            bool seen_a = false;
            bool seen_b = false;
            for (auto &a : chain_a) {
                if (canSee(i, j, a.i, a.j)) {
                    seen_a = true;
                    break;
                }
            }
            for (auto &b : chain_b) {
                if (canSee(i, j, b.i, b.j)) {
                    seen_b = true;
                    break;
                }
            }
            if (seen_a && seen_b) {
                bool should_ret = false;
                for (auto unset : getSetBits(marks)) {
                    Move move = {(char)(unset + START_CHAR), i, j, &Board::pencil};
                    moves.push_back(move);
                    should_ret = true;
                }
                if (should_ret) return true;
            }
        }
    }
    return false;
}

/**
 * @brief Looks for a chain of pairs
 * 
 * @param board to search through
 * @param num pair to search for. Each digit in the pair is a set bit in num
 * @param moves vector that gets populated with found moves
 * @return true if moves are found
 * @return false if moves are not found
 */
bool findChainOfPairs(Board &board, const std::uint16_t num, std::vector<Move> &moves) {
    std::vector<Coord> all_doubles;
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            if (!board.isEmpty(i, j)) continue;
            if (board.getPencil(i, j) != num) continue;
            // Double in this location
            Coord here = {(char)i, (char)j};
            all_doubles.push_back(here);
        }
    }
    if (all_doubles.size() < 3) return false;
    int chain_idx = 0;
    for (auto link : all_doubles) {
        auto chain = all_doubles;
        std::vector<Coord> chain_a, chain_b;
        chain_a.push_back(link);
        chain.erase(chain.begin() + chain_idx++);
        while (build_chain(chain_a, chain_b, chain)) {};
        if (chain_a.size() + chain_b.size() < 4) continue;
        if (removedByChain(board, num, chain_a, chain_b, moves)) {
            return true;
        }
    }
    return false;
}
