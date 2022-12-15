#include "HumanSolve.h"
#include <map>

typedef struct {
    int i;
    int j;
} coords;

bool solveHuman(Board *board) {
    static bool autoPencil = true;
    if (autoPencil) {
        board->autoPencil();
        autoPencil = false;
    }
    return findNakedSingles(board);
}

bool findNakedSingles(Board *board){
    auto marks = board->getPencilMarks();
    auto grid = board->getPlayGrid();

    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            if (grid[i][j] != 0) continue;
            for (unsigned char num = 0; num < 9; num++) {
                if ((marks[i][j] | (1 << num)) == (1 << num)) {
                    bool correct = board->insert(num + START_CHAR, i, j);
                    return true;
                }
            }
        }
    }

    return findHiddenSingles(board);
}

bool findHiddenSingles(Board *board) {
    auto marks = board->getPencilMarks();
    auto grid = board->getPlayGrid();
    bool correct = true;

    // [box_idx][value] = count
    for (auto i_start = 0; i_start < 9; i_start += 3) {
        for (auto j_start = 0; j_start < 9; j_start += 3) {
            // loop through each box
            char count[9] = {0};
            for (auto i = i_start; i < i_start + 3; i++) {
                for (auto j = j_start; j < j_start + 3; j++) {
                    if (grid[i][j] != 0) continue;
                    for (unsigned char num = 0; num < 9; num++) {
                        if ((marks[i][j] & (1 << num)) != 0) {
                            count[num]++;
                        }
                    }
                }
            }
            for (auto c = 0; c < 9; c++) {
                if (count[c] != 1) continue;
                for (auto i = i_start; i < i_start + 3; i++) {
                    for (auto j = j_start; j < j_start + 3; j++) {
                        if (grid[i][j] != 0) continue;
                        if ((marks[i][j] & (1 << c)) != 0) {
                            correct = board->insert(c + START_CHAR, i, j);
                            return true;
                        }
                    }
                }
            }
        }
    }
    
    // looking for singles in rows and columns
    for (auto i = 0; i < 9; i++) {
        char count[2][9] = {};
        const int i_idx = 0; // first index is changing
        const int j_idx = 1; // second index is changing
        for (auto j = 0; j < 9; j++) {
            for (unsigned char num = 0; num < 9; num++) {
                if (((marks[i][j] & (1 << num)) != 0) && grid[i][j] == 0) {
                    count[j_idx][num]++;
                }
                if (((marks[j][i] & (1 << num)) != 0) && grid[j][i] == 0) {
                    count[i_idx][num]++;
                }
            }
        }
        for (auto j = 0; j < 2; j++) {
            for (unsigned char num = 0; num < 9; num++) {
                if (count[j][num] != 1) continue;
                for (auto k = 0; k < 9; k++) {
                    if (j == j_idx) {
                        if (grid[i][k] != 0) continue;
                        if ((marks[i][k] & (1 << num)) == 0) continue;
                        correct = board->insert(num + START_CHAR, i, k);
                        return true;
                    }
                    else if (j == i_idx){
                        if (grid[k][i] != 0) continue;
                        if ((marks[k][i] & (1 << num)) == 0) continue;
                        correct = board->insert(num + START_CHAR, k, i);
                        return true;
                    }
                }
            }
        }
    }

    return findPointingBox(board);
}

static bool removeMarks_i_box(Board *board, char val, int i, int j_box) {
    auto grid = board->getPlayGrid();
    auto pencil = board->getPencilMarks();
    for (auto idx = 0; idx < 9; idx++) {
        if (((idx / 3) * 3) == j_box) continue; // same box as pointers
        if (grid[i][idx] != 0) continue;
        if ((pencil[i][idx] & (1 << (val - START_CHAR))) != 0) {
            bool correct = board->pencil(val, i, idx);
            return true;
        }
    }
    return false;
}

static bool removeMarks_j_box(Board *board, char val, int j, int i_box) {
    auto grid = board->getPlayGrid();
    auto pencil = board->getPencilMarks();
    for (auto idx = 0; idx < 9; idx++) {
        if (((idx / 3) * 3) == i_box) continue;
        if (grid[idx][j] != 0) continue;
        if ((pencil[idx][j] & (1 << (val - START_CHAR))) != 0) {
            bool correct = board->pencil(val, idx, j);
            return true;
        }
    }
    return false;
}

static bool findPointingBox(Board *board, int row, int col) {
    std::vector<coords> data[9];

    auto pencil = board->getPencilMarks();

    for (auto i = row; i < row + 3; i++) {
        for (auto j = col; j < col + 3; j++) {
            if (board->getPlayGrid()[i][j] != 0) continue;
            for (unsigned char num = 0; num < 9; num++) {
                if ((pencil[i][j] & (1 << num)) != 0) {
                    data[num].push_back({i,j});
                }
            }
        }
    }

    for (auto i = 0; i < 9; i++) {
        if (data[i].size() == 0) continue;
        bool i_check = true;
        bool j_check = true;
        int i_val = data[i][0].i;
        int j_val = data[i][0].j;
        if (data[i].size() > 3) continue;
        for (auto j = 1; j < data[i].size(); j++) {
            if (data[i][j].i != i_val) i_check = false;
            if (data[i][j].j != j_val) j_check = false;
            if (!i_check && !j_check) continue;
        }
        if (i_check) {
            if (removeMarks_i_box(board, i+1, i_val, col)) {
                return true;
            }
        }
        if (j_check) {
            if (removeMarks_j_box(board, i+1, j_val, row)) {
                return true;
            }
        }
    }
    return false;
}

bool findPointingBox(Board *board) {
    for (auto i = 0; i < 9; i+=3) {
        for (auto j = 0; j < 9; j+=3) {
            if (findPointingBox(board, i, j)) {
                return true;
            }
        }
    }
    return findNakedDoubles(board);
}

static bool removeDoublesInBox(Board *board, int i_1, int j_1, int i_2, int j_2, char x, char y) {
    bool removedPencil = false;
    bool correct = true;
    int box_i = (i_1 / 3) * 3;
    int box_j = (j_1 / 3) * 3;
    const int box_i_lim = box_i + 3;
    const int box_j_lim = box_j + 3;
    auto pencils = board->getPencilMarks();
    auto grid = board->getPlayGrid();
    for (box_i; box_i < box_i_lim; box_i++) {
        for (box_j; box_j < box_j_lim; box_j++) {
            if (grid[box_i][box_j] != 0) continue;
            if ((box_i == i_1 && box_j == j_1) || (box_i == i_2 && box_j == j_2))  continue;
            //if ((pencils[box_i][box_j] & ((1 << (x - START_CHAR))|(1 << (y - START_CHAR)))) != 0) {
            if ((pencils[box_i][box_j] & (1 << (x - START_CHAR))) != 0) {
                correct = board->pencil(x, box_i, box_j);
                removedPencil = true;
            }
            if ((pencils[box_i][box_j] & (1 << (y - START_CHAR))) != 0) {
                correct = board->pencil(y, box_i, box_j);
                removedPencil = true;
            }
        }
    }
    return removedPencil;
}

static void setXYFromBits(std::uint16_t bits, char *x, char *y) {
    bool foundFirst = false;
    for (unsigned char i = 0; i < 9; i++) {
        if ((bits & (1 << i)) != 0) {
            if (!foundFirst) {
                foundFirst = true;
                *x = i + START_CHAR;
            }
            else {
                *y = i + START_CHAR;
                break;
            }
        }
    }
}

static bool findDoublesInBox(Board *board, int i, int j, std::uint16_t bits) {
    int box_i = (i / 3) * 3;
    int box_j = (j / 3) * 3;
    const int box_i_lim = box_i + 3;
    const int box_j_lim = box_j + 3;
    auto pencils = board->getPencilMarks();
    auto grid = board->getPlayGrid();
    for (box_i; box_i < box_i_lim; box_i++) {
        for (box_j; box_j < box_j_lim; box_j++) {
            if (box_j == j && box_i == i) continue;
            if (grid[box_i][box_j] != 0) continue;
            if (pencils[box_i][box_j] == bits) {
                char x = 0;
                char y = 0;
                setXYFromBits(bits, &x, &y);
                if (removeDoublesInBox(board, i, j, box_i, box_j, x, y)) {
                    return true;
                }
            }
        }
    }
    return false;
}

static bool removeDoublesInI(Board *board, int i_1, int j, int i_2, char x, char y) {
    auto pencils = board->getPencilMarks();
    auto grid = board->getPlayGrid();
    bool removedPencil = false;
    bool correct = true;
    for (auto idx = 0; idx < 9; idx++) {
        if (idx == i_1 || idx == i_2) continue;
        if (grid[idx][j] != 0) continue;
        if ((pencils[idx][j] & (1 << (x - START_CHAR))) != 0) {
            correct = board->pencil(x, idx, j);
        }
        if ((pencils[idx][j] & (1 << (y - START_CHAR))) != 0) {
            correct = board->pencil(y, idx, j);
        }
        removedPencil = ((pencils[idx][j] & ((1 << (x - START_CHAR)) | (1 << (x - START_CHAR)))) != 0);
    }
    return removedPencil;
}

static bool findDoublesInI(Board *board, int i, int j, std::uint16_t bits) {
    auto pencils = board->getPencilMarks();
    for (auto idx = 0; idx < 9; idx++) {
        if (board->getPlayGrid()[idx][j] != 0) continue;
        if (idx == i) continue;
        if (pencils[idx][i] == bits) {
            char x = 0;
            char y = 0;
            setXYFromBits(bits, &x, &y);
            if (removeDoublesInI(board, i, j, idx, x, y)) {
                return true;
            }
        }
    }
    return false;
}

static bool removeDoublesInJ(Board *board, int i, int j_1, int j_2, char x, char y) {
    auto pencils = board->getPencilMarks();
    bool removedPencils = false;
    bool correct = true;
    for (auto idx = 0; idx < 9; idx++) {
        if (idx == j_1 || idx == j_2) continue;
        if (board->getPlayGrid()[i][idx] != 0) continue;
        if ((pencils[i][idx] & (1 << (x - START_CHAR))) != 0) {
            correct = board->pencil(x, i, idx);
            removedPencils = true;
        }
        if ((pencils[i][idx] & (1 << (y - START_CHAR))) != 0) {
            correct = board->pencil(y, i, idx);
            removedPencils = true;
        }
    }
    return removedPencils;
}

static bool findDoublesInJ(Board *board, int i, int j, std::uint16_t bits) {
    auto pencils = board->getPencilMarks();
    for (auto idx = 0; idx < 9; idx++) {
        if (idx == j) continue;
        if (board->getPlayGrid()[i][idx] != 0) continue;
        if (pencils[i][idx] == bits) {
            char x = 0;
            char y = 0;
            setXYFromBits(bits, &x, &y);
            if (removeDoublesInJ(board, i, j, idx, x, y)) {
                return true;
            }
        }
    }
    return false;
}

bool findNakedDoubles(Board *board) {
    auto grid = board->getPlayGrid();
    auto pencils = board->getPencilMarks();
    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            if (grid[i][j] > 0) continue;
            int count = 0;
            char x = 0;
            char y = 0;
            for (unsigned char num = 0; num < 9; num++) {
                if ((pencils[i][j] & (1 << num)) != 0) {
                    count++;
                }
            }
            if (count == 2) {
                if (findDoublesInBox(board, i, j, pencils[i][j])) {
                    return true;
                }
                if (findDoublesInI(board, i, j, pencils[i][j])) {
                    return true;
                }
                if (findDoublesInJ(board, i, j, pencils[i][j])) {
                    return true;
                }
            }
        }
    }
    return false;
}
