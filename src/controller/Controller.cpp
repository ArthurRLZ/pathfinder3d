#include "controller/Controller.h"
#include "model/BFS.h"
#include <iostream>

Controller::Controller(Grid& g)
    : grid(g), startPos(-1, -1), goalPos(-1, -1), mode(EditMode::None) {}

void Controller::onKey(unsigned char key) {
    switch (key) {
        case 's': case 'S':
            mode = EditMode::Start;
            std::cout << "Modo START\n";
            break;
        case 'g': case 'G':
            mode = EditMode::Goal;
            std::cout << "Modo GOAL\n";
            break;
        case 'w': case 'W':
            mode = EditMode::Wall;
            std::cout << "Modo WALL\n";
            break;
        case 'e': case 'E':
            mode = EditMode::Erase;
            std::cout << "Modo ERASE\n";
            break;
        case 13: // ENTER
            runBFS();
            break;
        case 'r': case 'R':
            resetGrid();
            break;
    }
}

void Controller::onMouse(int x, int y) {
    if (!grid.isInside(x, y)) return;

    Cell c(x, y);

    switch (mode) {
        case EditMode::Start:
            if (startPos.x >= 0)
                grid.set(startPos.x, startPos.y, CellType::Empty);
            startPos = c;
            grid.set(c.x, c.y, CellType::Start);
            break;

        case EditMode::Goal:
            if (goalPos.x >= 0)
                grid.set(goalPos.x, goalPos.y, CellType::Empty);
            goalPos = c;
            grid.set(c.x, c.y, CellType::Goal);
            break;

        case EditMode::Wall:
            grid.set(c.x, c.y, CellType::Wall);
            break;

        case EditMode::Erase:
            grid.set(c.x, c.y, CellType::Empty);
            break;

        default:
            break;
    }
}

void Controller::runBFS() {
    if (startPos.x < 0 || goalPos.x < 0) {
        std::cout << "Defina START e GOAL primeiro!\n";
        return;
    }

    for (int y = 0; y < grid.getHeight(); y++) {
        for (int x = 0; x < grid.getWidth(); x++) {
            if (grid.get(x, y) == CellType::Visited ||
                grid.get(x, y) == CellType::Path) {
                grid.set(x, y, CellType::Empty);
            }
        }
    }

    std::vector<Cell> path;
    if (BFS::run(grid, startPos, goalPos, path)) {
        for (auto& c : path) {
            if (grid.get(c.x, c.y) == CellType::Empty)
                grid.set(c.x, c.y, CellType::Path);
        }
        std::cout << "Caminho encontrado!\n";
    } else {
        std::cout << "Sem caminho.\n";
    }
}

void Controller::resetGrid() {
    for (int y = 0; y < grid.getHeight(); y++)
        for (int x = 0; x < grid.getWidth(); x++)
            grid.set(x, y, CellType::Empty);

    startPos = {-1, -1};
    goalPos = {-1, -1};
    std::cout << "Grid resetado\n";
}