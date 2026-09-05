#include "controller/Controller.h"
#include "model/BFS.h"
#include "model/AStar.h"
#include <iostream>
#include <GL/glut.h>

Controller::Controller(Grid& g)
    : grid(g), startPos(-1, -1), goalPos(-1, -1), mode(EditMode::None), currentAlgorithm(AlgorithmType::BFS), camera(nullptr) {}

void Controller::setCamera(Camera* cam) {
    camera = cam;
}

void Controller::onKey(unsigned char key) {
    switch (key) {
        case '1':
            currentAlgorithm = AlgorithmType::BFS;
            std::cout << "Algoritmo selecionado: BFS\n";
            break;
        case '2':
            currentAlgorithm = AlgorithmType::ASTAR;
            std::cout << "Algoritmo selecionado: A-Star\n";
            break;
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
        case 'a': case 'A':
            if (camera) camera->ProcessMouseMovement(-15.0f, 0.0f);
            break;
        case 'd': case 'D':
            if (camera) camera->ProcessMouseMovement(15.0f, 0.0f);
            break;
        case 13: // ENTER
            runAlgorithm();
            break;
        case 'r': case 'R':
            resetGrid();
            break;
    }
}

void Controller::onSpecialKey(int key) {
    if (!camera) return;
    float velocity = 0.2f;
    switch (key) {
        case GLUT_KEY_UP: camera->ProcessKeyboard(CameraMovement::FORWARD, velocity); break;
        case GLUT_KEY_DOWN: camera->ProcessKeyboard(CameraMovement::BACKWARD, velocity); break;
        case GLUT_KEY_LEFT: camera->ProcessKeyboard(CameraMovement::LEFT, velocity); break;
        case GLUT_KEY_RIGHT: camera->ProcessKeyboard(CameraMovement::RIGHT, velocity); break;
    }
}

void Controller::onMouse(int x, int y) {
    if (!grid.isInside(x, y)) return;
    switch (mode) {
        case EditMode::Wall:
            if (grid.get(x, y) == CellType::Wall) grid.set(x, y, CellType::Empty);
            else if (grid.get(x, y) == CellType::Empty) grid.set(x, y, CellType::Wall);
            break;
        case EditMode::Erase:
            if (grid.get(x, y) != CellType::Start && grid.get(x, y) != CellType::Goal) 
                grid.set(x, y, CellType::Empty);
            break;
        case EditMode::Start: setStart(x, y); break;
        case EditMode::Goal: setGoal(x, y); break;
        default: break;
    }
}

void Controller::setStart(int x, int y) {
    if (grid.get(x, y) == CellType::Wall) return;
    if (startPos.x >= 0) grid.set(startPos.x, startPos.y, CellType::Empty);
    startPos = {x, y};
    grid.set(x, y, CellType::Start);
}

void Controller::setGoal(int x, int y) {
    if (grid.get(x, y) == CellType::Wall) return;
    if (goalPos.x >= 0) grid.set(goalPos.x, goalPos.y, CellType::Empty);
    goalPos = {x, y};
    grid.set(x, y, CellType::Goal);
}

void Controller::runAlgorithm() {
    if (startPos.x < 0 || goalPos.x < 0) {
        std::cout << "Defina START e GOAL primeiro!\n";
        return;
    }

    for (int y = 0; y < grid.getHeight(); y++) {
        for (int x = 0; x < grid.getWidth(); x++) {
            if (grid.get(x, y) == CellType::Visited || grid.get(x, y) == CellType::Path) {
                grid.set(x, y, CellType::Empty);
            }
        }
    }

    std::vector<Cell> path;
    bool found = false;

    if (currentAlgorithm == AlgorithmType::BFS) {
        found = BFS::run(grid, startPos, goalPos, path);
    } else {
        found = AStar::run(grid, startPos, goalPos, path);
    }

    if (!found) {
        std::cout << "Sem caminho.\n";
        return;
    }

    for (int y = 0; y < grid.getHeight(); y++) {
        for (int x = 0; x < grid.getWidth(); x++) {
            if (grid.get(x, y) == CellType::Visited) grid.set(x, y, CellType::Empty);
        }
    }

    for (auto& c : path) {
        if (grid.get(c.x, c.y) == CellType::Empty) grid.set(c.x, c.y, CellType::Path);
    }

    std::cout << "Caminho encontrado! (" << path.size() << " passos)\n";
}

void Controller::resetGrid() {
    for (int y = 0; y < grid.getHeight(); y++)
        for (int x = 0; x < grid.getWidth(); x++)
            grid.set(x, y, CellType::Empty);
    startPos = {-1, -1};
    goalPos = {-1, -1};
    std::cout << "Grid resetado\n";
}