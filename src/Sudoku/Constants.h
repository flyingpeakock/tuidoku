#pragma once

namespace Sudoku {
    enum constraintColTypes {
        eConstraintCell = 0,
        eConstraintRow,
        eConstraintCol,
        eConstraintBox,
        eConstraintTypes
    };

    enum {
        eSize = 9,
        eBoxSize = 3,
        eBoardSize = eSize * eSize,
        eConstraints = eBoardSize * eConstraintTypes,
        eBufferSize = eConstraints * eSize,
    };
} // namespace Sudoku

