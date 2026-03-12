#pragma once

#include "model/Grid.h"
#include "model/Cell.h"
#include "view/Camera.h"

enum class EditMode { None, Start, Goal, Wall, Erase };
enum class AlgorithmType { BFS, ASTAR };

class Controller {
public:
    Controller(Grid& grid);
    void setCamera(Camera* cam);
    void setStart(int x, int y);
    void setGoal(int x, int y);
    void onKey(unsigned char key);
    void onSpecialKey(int key);
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
    AlgorithmType currentAlgorithm;
    Camera* camera;

    void runAlgorithm();
    void resetGrid();
};