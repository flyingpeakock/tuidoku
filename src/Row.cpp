#include "Row.h"

int row::operator[](int i) {
    switch(i) {
        case 0:
            return col1;
        case 1:
            return col2;
        case 2:
            return col3;
        case 3:
            return col4;
        case 4:
            return col5;
        case 5:
            return col6;
        case 6:
            return col7;
        case 7:
            return col8;
        case 8:
            return col9;
    }
    return -1;
}

void row::set(int col, int val) {
    switch(col) {
        case 0:
            col1 = val;
            return;
        case 1:
            col2 = val;
            return;
        case 2:
            col3 = val;
            return;
        case 3:
            col4 = val;
            return;
        case 4:
            col5 = val;
            return;
        case 5:
            col6 = val;
            return;
        case 6:
            col7 = val;
            return;
        case 7:
            col8 = val;
            return;
        case 8:
            col9 = val;
            return;
    }
}

int row::size() {
    return 9;
}