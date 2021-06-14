#pragma once
#include <array>

struct row {
    int col1:5;
    int col2:5;
    int col3:5;
    int col4:5;
    int col5:5;
    int col6:5;
    int col7:5;
    int col8:5;
    int col9:5;
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