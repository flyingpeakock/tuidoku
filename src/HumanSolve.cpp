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
            if (grid[i][j] != 0) {
                continue;
            }
            std::vector<char> non_emptymarks;
            for (auto m : marks[i][j]) {
                if (m != ' ') {
                    non_emptymarks.push_back(m);
                }
            }
            if (non_emptymarks.size() == 1)
            {
                board->insert(non_emptymarks[0], i, j);
                return true;
            }
        }
    }

    return findHiddenSingles(board);
}

bool findHiddenSingles(Board *board) {
    auto marks = board->getPencilMarks();
    auto grid = board->getPlayGrid();

    for (auto i = 0; i < 9; i++) {
        for (auto j = 0; j < 9; j++) {
            if (grid[i][j] != 0) {
                continue;
            }

            auto box_i = (i / 3) * 3;
            auto box_j = (j / 3) * 3;
            auto num_idx = 0;
            
            // looping through all the marks at this index
            for (auto num : marks[i][j]) {
                if (num < 1) continue;
                int count = 0;
                // single in box
                for (auto k = box_i; k < box_i + 3; k++) {
                    for (auto l = box_j; l < box_j + 3; l++) {
                        for (auto check_num : marks[k][l]) {
                            if (num == check_num) {
                                count++;
                            }
                        }
                    }
                }
                if (count == 1) {
                    board->insert(marks[i][j][num_idx], i, j);
                    return true;
                }
                count = 0;
                // checking first index
                for (auto k = 0; k < 9; k++) {
                    for (auto check_num : marks[k][j]) {
                        if (num == check_num) {
                            count++;
                        }
                    }
                }
                if (count == 1) {
                    board->insert(marks[i][j][num_idx], i, j);
                    return true;
                }

                // checking second index
                count = 0;
                for (auto k = 0; k < 9; k++) {
                    for (auto check_num : marks[i][k]) {
                        if (num == check_num) {
                            count++;
                        }
                    }
                }
                if (count == 1) {
                    board->insert(marks[i][j][num_idx], i, j);
                }
                num_idx++;
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
        for (auto num : pencil[i][idx]) {
            if (num == val) {
                board->pencil(val, i, idx);
                return true;
            }
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
        for (auto num : pencil[idx][j]) {
            if (num == val) {
                board->pencil(val, idx, j);
                return true;
            }
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
            for (auto num : pencil[i][j]) {
                if (num <= '9' && num >= '1') {
                    num -= '0';
                }
                if (num > 9 || num < 1) {
                    continue;
                }
                data[num - 1].push_back({i,j});
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
            for (auto num: pencils[box_i][box_j]) {
                if (num == x || num == y) {
                    board->pencil(num, box_i, box_j);
                    removedPencil = true;
                }
            }
        }
    }
    return removedPencil;
}

static bool findDoublesInBox(Board *board, int i, int j, char x, char y) {
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
            int count = 0;
            for (auto num : pencils[box_i][box_j]) {
                if (num == ' ') continue;
                count++;
                if (num != x && num != y) {
                    count = 3;
                }
            }
            if (count == 2) {
                if (removeDoublesInBox(board, i, j, box_i, box_j, x, y)) {
                    return true;
                }
            }
        }
    }
    return false;
}

static bool removeDoublesInI(Board *board, int i_1, int j, int i_2, int x, int y) {
    auto pencils = board->getPencilMarks();
    auto grid = board->getPlayGrid();
    bool removedPencil = false;
    for (auto idx = 0; idx < 9; idx++) {
        if (idx == i_1 || idx == i_2) continue;
        if (grid[idx][j] != 0) continue;
        for (auto num : pencils[idx][j]) {
            if (num == x || num == y) {
                board->pencil(num, idx, j);
                removedPencil = true;
            }
        }
    }
    return removedPencil;
}

static bool findDoublesInI(Board *board, int i, int j, char x, char y) {
    auto pencils = board->getPencilMarks();
    for (auto idx = 0; idx < 9; idx++) {
        if (board->getPlayGrid()[idx][j] != 0) continue;
        if (idx == i) continue;
        int count = 0;
        bool sameDouble = false;
        for (auto num : pencils[idx][j]) {
            if (num != ' ') {
                count++;
                if (num != x && num != y) {
                    sameDouble = false;
                }
            }
        }
        if (count == 2 && sameDouble) {
            if (removeDoublesInI(board, i, j, idx, x, y)) {
                return true;
            }
        }
    }
    return false;
}

static bool removeDoublesInJ(Board *board, int i, int j_1, int j_2, int x, int y) {
    auto pencils = board->getPencilMarks();
    bool removedPencils = false;
    for (auto idx = 0; idx < 9; idx++) {
        if (idx == j_1 || idx == j_2) continue;
        if (board->getPlayGrid()[i][idx] != 0) continue;
        for (auto num : pencils[i][idx]) {
            if (num == x || num == y) {
                board->pencil(num, i, idx);
                removedPencils = true;
            }
        }
    }
    return removedPencils;
}

static bool findDoublesInJ(Board *board, int i, int j, char x, char y) {
    auto pencils = board->getPencilMarks();
    for (auto idx = 0; idx < 9; idx++) {
        if (idx == j) continue;
        if (board->getPlayGrid()[i][idx] != 0) continue;
        int count = 0;
        bool sameDouble = true;
        for (auto num : pencils[i][idx]) {
            if (num != ' ') {
                count++;
                if (num != x || num != y) {
                    sameDouble = false;
                }
            }
        }
        if (count == 2 && sameDouble) {
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
            for (auto num : pencils[i][j]) {
                if (num != ' ') {
                    count++;
                    if (x == 0) {
                        x = num;
                    }
                    else {
                        y = num;
                    }
                }
            }
            if (count == 2) {
                if (findDoublesInBox(board, i, j, x, y)) {
                    return true;
                }
                if (findDoublesInI(board, i, j, x, y)) {
                    return true;
                }
                if (findDoublesInJ(board, i, j, x, y)) {
                    return true;
                }
            }
        }
    }
    return false;
}
