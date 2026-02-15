#pragma once
#include <vector>
#include "CellType.h"

class Grid {
private:
    int width;
    int height;
    std::vector<std::vector<CellType>> cells;

public:
    Grid(int w, int h);

    CellType get(int x, int y) const;
    void set(int x, int y, CellType value);

    bool isInside(int x, int y) const;

    int getWidth() const;
    int getHeight() const;
};
