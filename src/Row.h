#pragma once
#include <array>

struct row {
    unsigned int col1:4;
    unsigned int col2:4;
    unsigned int col3:4;
    unsigned int col4:4;
    unsigned int col5:4;
    unsigned int col6:4;
    unsigned int col7:4;
    unsigned int col8:4;
    unsigned int col9:4;
    int operator[](int i);
    int size();
    void set(int col, int val);
};

inline bool operator==(const row &l, const row &r) {
    return (l.col1 == r.col1 && l.col2 == r.col2 && l.col3 == r.col3 &&
            l.col4 == r.col4 && l.col5 == r.col5 && l.col6 == r.col6 &&
            l.col7 == r.col7 && l.col8 == r.col8 && l.col9 == r.col9);
};

typedef std::array<row, 9> puzzle;