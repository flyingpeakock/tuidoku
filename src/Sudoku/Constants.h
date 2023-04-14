#pragma once

namespace Sudoku {
    enum {
        eSize = 9,
        eBoxSize = 3,
        eBoardSize = eSize * eSize,
        eConstraints = eBoardSize * 4,
        eBufferSize = eConstraints * eSize,
    };
} // namespace Sudoku

