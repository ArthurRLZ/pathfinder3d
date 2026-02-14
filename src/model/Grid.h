
#pragma once
#include <vector>

class Grid {
private:
    int width;
    int height;
    std::vector<std::vector<int>> cells;

public:
    Grid(int w, int h);

    int get(int x, int y) const;
    void set(int x, int y, int value);

    bool isInside(int x, int y) const;

    int getWidth() const;
    int getHeight() const;
};

