#include "Grid.h"

Grid::Grid(int w, int h) : width(w), height(h) {
    cells.resize(height, std::vector<int>(width, 0));
}

int Grid::get(int x, int y) const {
    return cells[y][x];
}

void Grid::set(int x, int y, int value) {
    cells[y][x] = value;
}

bool Grid::isInside(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

int Grid::getWidth() const {
    return width;
}

int Grid::getHeight() const {
    return height;
}