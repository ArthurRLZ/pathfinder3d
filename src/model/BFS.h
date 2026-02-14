
#pragma once
#include <vector>
#include "Grid.h"
#include "Cell.h"

class BFS {
public:
    static bool run(
        Grid& grid,
        Cell start,
        Cell goal,
        std::vector<Cell>& outPath
    );
};

