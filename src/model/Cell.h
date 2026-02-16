#pragma once

struct Cell {
    int x;
    int y;

    Cell(int _x = 0, int_y = 0) : x(_x), y(_y) {}

    bool operator==(const Cell& other) const {
        return x== other.x && y == other.y;
    }

    bool operator!=(const Cell& other) const {
        return !(*this == other);
    }

};

