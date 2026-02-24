#pragma once

#include "model/Grid.h"
#include "model/Cell.h"

enum class EditMode {
    None,
    Start,
    Goal,
    Wall,
    Erase
};

class Controller {
public:
    Controller(Grid& grid);

    void onKey(unsigned char key);
    void onMouse(int gridX, int gridY);

    void setStart(const Cell& c);
    void setGoal(const Cell& c);
    void addWall(const Cell& c);
    void clearCell(const Cell& c);

private:
    Grid& grid;
    Cell startPos;
    Cell goalPos;
    EditMode mode;

    void runBFS();
    void resetGrid();
};