#include "HumanSolve.h"
#include <map>
#include <unordered_set>
#include <utility>
#include <algorithm>

static void getCombinations(int offset, int k, std::vector<char> numbrs, std::uint16_t combinations, std::unordered_set<std::uint16_t> *final) {
    if (k == 0) {
        (*final).emplace(combinations);
        return;
    }

    for (int i = offset; i <= numbrs.size(); i++) {
        combinations |= (1 << numbrs[i]);
        getCombinations(i + 1, k - 1, numbrs, combinations, final);
        combinations &= ~(1 << numbrs[i]);
    }
}

static std::unordered_set<uint16_t> getCombinations(Board *board, int k) {
    std::unordered_set<uint16_t> final;
    std::vector<char> valid_nums;
    for (unsigned char c = 0; c < 9; c++) {
        if (board->isRemaining((int)c + 1)){
            valid_nums.push_back(c);
        }
    }
    getCombinations(0, k, valid_nums, 0, &final);
    return final;
}

static int countBits(std::uint16_t bits) {
    int count = 0;
    while (bits != 0) {
        if (bits & 1 == 1) {
            count++;
        }
        bits >>=1;
    }
    return count;
}

 static std::vector<unsigned char> getSetBits(std::uint16_t bits) {
    std::vector<unsigned char> nums;
    for (unsigned char num = 0; num < 9; num++) {
        if ((bits & (1 << num)) != 0) {
            nums.push_back(num);
        }
    }
    return nums;
 }

static char countOccurrencesNaked(Board *board, std::uint16_t bits, char i_min, char i_max, char j_min, char j_max) {
    char count = 0;
    for (auto i = i_min; i < i_max; i++) {
        for (auto j = j_min; j < j_max; j++) {
            if (!board->isEmpty(i, j)) continue;
            std::uint16_t marks = board->getPencil(i, j);
            if ((marks & bits) == 0) continue; // does not exist here
            if ((marks | bits) == bits) count++; // only exists here
        }
    }
    return count;
}

static char countOccurrencesHidden(Board *board, std::uint16_t bits, char i_min, char i_max, char j_min, char j_max) {
   char count = 0;
   std::uint16_t seen = 0;
   for (auto i = i_min; i < i_max; i++) {
        for (auto j = j_min; j < j_max; j++) {
            if (!board->isEmpty(i, j)) continue;
            auto marks = board->getPencil(i ,j);
            if ((marks & bits) != 0) {
                count++;
                seen |= (marks & bits);
            }
        }
    }
    if (seen != bits) return 0xFF;
    return count;
}

static bool removedOccurrencesNaked(Board *board, std::uint16_t bits, char i_min, char i_max, char j_min, char j_max) {
    bool ret = false;
    for (auto i = i_min; i < i_max; i++) {
        for (auto j = j_min; j < j_max; j++) {
            if (!board->isEmpty(i, j)) continue;
            std::uint16_t marks = board->getPencil(i, j);
            if ((marks & ~(bits)) == 0) continue; // Only marks are set here
            marks &= bits; // marks == bits that should be removed
            if ((marks) == 0) continue; // no marks set here
            auto set_bits = getSetBits(marks);
            for (auto &num : set_bits) {
                board->pencil(num + START_CHAR, i, j);
                ret = true;
            }
        }
    }
    return ret;
}

static bool removedOccurrencesHidden(Board *board, std::uint16_t bits, char i_min, char i_max, char j_min, char j_max) {
    bool ret = false;
    for (auto i = i_min; i < i_max; i++) {
        for (auto j = j_min; j < j_max; j++) {
            if (!board->isEmpty(i, j)) continue;
            std::uint16_t marks = board->getPencil(i, j);
            if ((marks & bits) == 0) continue;
            marks &= ~bits;
            auto set_bits = getSetBits(marks);
            for (auto &num : set_bits) {
                board->pencil(num + START_CHAR, i, j);
                ret = true;
            }
        }
    }
    return ret;
}


bool solveHuman(Board *board) {
    static Board *lastBoard = NULL;
    if (board != lastBoard) {
        board->autoPencil();
        lastBoard = board;
    }
    if (board->isWon()) return false;

    auto all_singles = getCombinations(board, 1);
    for (auto &num : all_singles) {
        if (findNakedSingles(board, num)) {
            return true;
        }
        if (findHiddenSingles(board, num)) {
            return true;
        }
    }
    
    for (char i = 0; i < 9; i += 3) {
        for (char j = 0; j < 9; j += 3) {
            if (findPointingBox(board, i, j)) {
                return true;
            }
        }
    }

    auto all_doubles = getCombinations(board, 2);
    for (auto num : all_doubles) {
        if (findNaked(board, num)) {
            return true;
        }
        if (findHidden(board, num)) {
            return true;
        }
    }

    if (findLockedCandidates(board)) {
        return true;
    }
    auto all_triples = getCombinations(board, 3);
    for (auto &triple : all_triples) {
        if (findNaked(board, triple)) {
            return true;
        }
    }
    auto all_quads = getCombinations(board, 4);
    for (auto &quad : all_quads) {
        if (findNaked(board, quad)) {
            return true;
        }
    }
    for (auto &triple : all_triples) {
        if (findHidden(board, triple)) {
            return true;
        }
    }
    for (auto &quad : all_quads) {
        if (findHidden(board, quad)) {
            return true;
        }
    }
    if (findBug(board)) {
        return true;
    }
    for (auto &single : all_singles) {
        if (findXwing(board, single)) {
            return true;
        }
    }
    for (auto &doub : all_doubles) {
        if (findUniqueRectangle(board, doub)) {
            return true;
        }
    }
    return false;
}

bool findNakedSingles(Board *board, const std::uint16_t single) {
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            if (!board->isEmpty(i, j)) continue;
            if (board->getPencil(i, j) == single) {
                board->insert(getSetBits(single)[0] + START_CHAR, i ,j);
                return true;
            }
        }
    }
    return false;
}

bool findHiddenSingles(Board *board, const std::uint16_t single) {
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            if (!board->isEmpty(i ,j)) continue;
            if ((board->getPencil(i, j) & single) == 0) continue;
            auto i_box = (i / 3) * 3;
            auto j_box = (j / 3) * 3;
            if ((countOccurrencesHidden(board, single, i_box, i_box + 3, j_box, j_box+3) == 1)
              || (countOccurrencesHidden(board, single, i, i+1, 0, 9) == 1)
              || (countOccurrencesHidden(board, single, 0, 9, j, j+1) == 1)) {
                board->insert(getSetBits(single)[0] + START_CHAR, i ,j);
                return true;
            }
        }
    }
    return false;
}

bool findNaked(Board *board, const std::uint16_t num) {
    if (num == 0) return false;
    const char matcher = countBits(num);
    // finding in box
    for (auto i_box = 0; i_box < 9; i_box += 3) {
        for (auto j_box = 0; j_box < 9; j_box += 3) {
            auto count = countOccurrencesNaked(board, num, i_box, i_box + 3, j_box, j_box + 3);
            if (count != matcher) continue;
            if (removedOccurrencesNaked(board, num, i_box, i_box + 3, j_box, j_box + 3)) {
                return true;
            }
        }
    }

    // rows and cols
    for (auto i = 0; i < 9; i++) {
        auto count =  countOccurrencesNaked(board, num, i, i+1, 0, 9);
        if (count == matcher) {
            if (removedOccurrencesHidden(board, num, i, i+1, 0, 9)) {
                return true;
            }
        }
        count = countOccurrencesNaked(board, num, 0, 9, i, i+1);
        if (count == matcher) {
            if (removedOccurrencesHidden(board, num, 0, 9, i, i+1)) {
                return true;
            }
        }
    }
    return false;
}


bool findHidden(Board *board, const std::uint16_t num) {
    if (num == 0) return false;
    const char matcher = countBits(num);
    // finding in box
    for (auto i_box = 0; i_box < 9; i_box += 3) {
        for (auto j_box = 0; j_box < 9; j_box += 3) {
            char count = countOccurrencesHidden(board, num, i_box, i_box + 3, j_box, j_box + 3);
            if (count != matcher) continue;
            if (removedOccurrencesHidden(board, num, i_box, i_box + 3, j_box, j_box + 3)) {
                return true;
            }
        }
    }

    // finding in rows and cols
    for (auto i = 0; i < 9; i++) {
        char count = countOccurrencesHidden(board, num, i, i+1, 0, 9);
        if (count == matcher) {
            if (removedOccurrencesHidden(board, num, i, i+1, 0, 9)) {
                return true;
            }
        }
        count = countOccurrencesHidden(board, num, 0, 9, i, i+1);
        if (count == matcher) {
            if (removedOccurrencesHidden(board, num, 0, 9, i, i+1)) {
                return true;
            }
        }
    }
    return false;
}

static bool removeMarks_i_box(Board *board, char val, int i, int j_box) {
    auto pencil = board->getPencilMarks();
    for (auto idx = 0; idx < 9; idx++) {
        if (((idx / 3) * 3) == j_box) continue; // same box as pointers
        if (!board->isEmpty(i, idx)) continue;
        if ((pencil[i][idx] & (1 << val)) != 0) {
            board->pencil(val + START_CHAR, i, idx);
            return true;
        }
    }
    return false;
}

static bool removeMarks_j_box(Board *board, char val, int j, int i_box) {
    auto pencil = board->getPencilMarks();
    for (auto idx = 0; idx < 9; idx++) {
        if (((idx / 3) * 3) == i_box) continue;
        if (!board->isEmpty(idx, j)) continue;
        if ((pencil[idx][j] & (1 << val)) != 0) {
            board->pencil(val + START_CHAR, idx, j);
            return true;
        }
    }
    return false;
}

bool findPointingBox(Board *board, char i_start, char j_start) {
    char counts[9] = {};
    bool indexes[9][3][3] = {false};
    auto marks = board->getPencilMarks();
    for (auto i = i_start; i < i_start + 3; i++) {
        for (auto j = j_start; j < j_start + 3; j++) {
            if (!board->isEmpty(i, j)) continue;
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
                if (removeMarks_i_box(board, num, i + i_start, j_start)) {
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
                if (removeMarks_j_box(board, num, j + j_start, i_start)) {
                    return true;
                };
            }
        }
    }
    return false;
}

static bool existsOnlyInBox(Board *board, char box_i, char box_j, char i_min, char i_max, char j_min, char j_max, char num) {
    auto pencils = board->getPencilMarks();
    for (auto i = i_min; i < i_max; i++) {
        for (auto j = j_min; j < j_max; j++) {
            if (!board->isEmpty(i, j)) continue;
            if (((i / 3) * 3) == box_i && ((j / 3) * 3) == box_j) continue;
            if ((pencils[i][j] & (1 << num)) != 0) {
                return false;
            }
        }
    }
    return true;
}

static bool removedLockedIFromBox(Board *board, char box_i, char box_j, char locked_i, char num) {
    auto pencils = board->getPencilMarks();
    bool ret = false;
    for (auto i = box_i; i < box_i + 3; i++) {
        for (auto j = box_j; j < box_j + 3; j++) {
            if (!board->isEmpty(i, j)) continue;
            if (i == locked_i) continue;
            if ((pencils[i][j] & (1 << num)) != 0) {
                board->pencil(num + START_CHAR, i, j);
                ret = true;
            }
        }
    }
    return ret;
}

static bool removedLockedJFromBox(Board *board, char box_i, char box_j, char locked_j, char num) {
    auto pencils = board->getPencilMarks();
    bool ret = false;
    for (auto i = box_i; i < box_i + 3; i++) {
        for (auto j = box_j; j < box_j + 3; j++) {
            if (!board->isEmpty(i, j)) continue;
            if (j == locked_j) continue;
            if ((pencils[i][j] & (1 << num)) != 0) {
                board->pencil(num + START_CHAR, i, j);
                ret = true;
            }
        }
    }
    return ret;
}

bool findLockedCandidates(Board *board) {
    auto pencils = board->getPencilMarks();
    for (auto box_i = 0; box_i < 9; box_i += 3) {
        for (auto box_j = 0; box_j < 9; box_j += 3) {
            for (auto i = box_i; i < box_i + 3; i++) {
                for (auto j = box_j; j < box_j + 3; j++) {
                    if (!board->isEmpty(i, j)) continue;
                    for (unsigned char num = 0; num < 9; num++) {
                        if ((pencils[i][j] & (1 << num)) == 0) continue;
                        if (existsOnlyInBox(board, box_i, box_j, i, i+1, 0, 9, num)) {
                            if (removedLockedIFromBox(board, box_i, box_j, i, num)) {
                                return true;
                            }
                        }
                        if (existsOnlyInBox(board, box_i, box_j, 0, 9, j, j+1, num)) {
                            if (removedLockedJFromBox(board, box_i, box_j, j, num)) {
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

bool findBug(Board *board) {
    int numb_of_threes = 0;
    int col, row;
    std::uint16_t bits = 0;
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            if (!board->isEmpty(i, j))continue;
            auto pencils = board->getPencil(i, j);
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
        if ((countOccurrencesHidden(board, num_bits, col, col + 1, 0, 9) == 3)
          && (countOccurrencesHidden(board, num_bits, 0, 9, row, row + 1) == 3)
          && (countOccurrencesHidden(board, num_bits, col_box, col_box + 3, row_box, row_box + 3))) {
            board->insert(num + START_CHAR, col, row);
            return true;
        }
    }
    return false;
}

static bool hasEqualwing(Board *board, std::uint16_t num, char occurrences, char *occurrences_ptr, bool is_i, std::uint16_t *positions) {
    if (occurrences == 0) return false;
    char positions_index = is_i ? 0 : 1;
    for (auto i = 0; i < 9; i++) {
        if (*(occurrences_ptr + i) == occurrences) {
            //(*(positions + positions_index)).emplace_back(i);
            (*(positions + positions_index)) |= (1 << i);
        }
    }
    //if ((*(positions + positions_index)).size() != occurrences) return false;
    if (countBits(*positions) != occurrences) return false;

    //for (auto i : (*(positions + positions_index))) {
    for (auto i : getSetBits(*(positions + positions_index))) {
        for (auto j = 0; j < 9; j++) {
            std::uint16_t mark;
            if (is_i) {
                if (!board->isEmpty(i, j)) continue;
                mark = board->getPencil(i, j);
            }
            else {
                if (!board->isEmpty(j, j)) continue;
                mark = board->getPencil(i, j);
            }
            if ((mark & num) == 0) continue;
            (*(positions + (!positions_index))) |= (1 << j);
        }
    }
    //return (*(positions + (!positions_index))).size() == occurrences;
    return countBits((*(positions + (!positions_index)))) == occurrences;
}

static bool removedByXwing(Board *board, std::uint16_t *positions, std::uint16_t num) {
    bool ret = false;
    // removing in j
    for (auto j : getSetBits(*(positions))) {
        for (auto i = 0; i < 9; i++) {
            if (!board->isEmpty(i, j)) continue;
            if ((*(positions + 1)) & (1 << i) == 0) continue;
            auto marks = board->getPencil(i, j);
            if ((marks & num) == 0) continue;
            board->pencil(getSetBits(num)[0] + START_CHAR, i, j);
            ret = true;
        }
    }

    // removing in i
    for (auto i : getSetBits(*(positions + 1))) {
        for (auto j = 0; j < 9; j++) {
            if (!board->isEmpty(i ,j)) continue;
            if ((*(positions)) & (1 << j) == 0) continue;
            auto marks = board->getPencil(i, j);
            if ((marks & num) == 0) continue;
            board->pencil(getSetBits(num)[0] + START_CHAR, i ,j);
            ret = true;
        }
    }
    return ret;
}

bool findXwing(Board *board, const std::uint16_t num) {
    char i_counts[9] = {};
    char j_counts[9] = {};
    for (auto i = 0; i < 9; i++) {
        i_counts[i] = countOccurrencesHidden(board, num, i, i+1, 0, 9);
        j_counts[i] = countOccurrencesHidden(board, num, 0, 9, i, i+1);
    }
    for (auto i = 0; i < 9; i++) {
        //std::vector<char> pos_vector[2];
        std::uint16_t indexes[2];
        if (hasEqualwing(board, num, i_counts[i], i_counts, true, indexes)) {
            if (removedByXwing(board, indexes, num)) {
                return true;
            }
        }
    }
    return false;
}

static bool findIntersection(Board *board, std::uint16_t num, char cur_i, char cur_j) {
    char i_1 = 0xFF;
    char j_1 = 0xFF;
    for (auto i = 0; i < 9; i++) {
        if (i == cur_i) continue;
        if (!board->isEmpty(i, cur_j)) continue;
        if ((board->getPencil(i, cur_j) & num) == num) {
            i_1 = i;
            break;
        }
    }
    for (auto j = 0; j < 9; j++) {
        if (j == cur_j) continue;
        if (!board->isEmpty(cur_i, j)) continue;
        if ((board->getPencil(cur_i, j) & num) == num) {
            j_1 = j;
            break;
        }
    }
    if (i_1 == 0xFF || j_1 == 0xFF) return false;
    if (!board->isEmpty(i_1, j_1)) return false;
    auto marks = board->getPencil(i_1, j_1) & num;
    bool ret = false;
    for (auto rem : getSetBits(marks)) {
        board->pencil(rem + START_CHAR, i_1, j_1);
        ret = true;
    }
    return ret;
}

bool findUniqueRectangle(Board *board, const std::uint16_t num) {
    if (num == 0) return false;
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            if (!board->isEmpty(i ,j)) continue;
            if ((board->getPencil(i, j)) != num) continue;
            if (countOccurrencesNaked(board, num, i, i+1, 0, 9) != 2) continue;
            if (countOccurrencesNaked(board, num, 0, 9, j, j+1) != 2) continue;
            if (findIntersection(board, num, i, j)) {
                return true;
            }
        }
    }
    return false;
}
