#pragma once
#include "model/Grid.h"
#include "model/Cell.h"
#include <vector>

class AStar {
public:
    static bool run(Grid& grid, Cell start, Cell goal, std::vector<Cell>& path);
};