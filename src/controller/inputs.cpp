// controler/inputs.cpp
#include "inputs.h"
#include <iostream>

#include "model/BFS.h"

void handleKeyboardInput(unsigned char key, int x, int y, Grid& grid, Cell& startPos, Cell& goalPos) {
    if (key == 'w') {
        grid.set(x, y, CellType::Wall);
    } else if (key == 'e') {
        grid.set(x, y, CellType::Empty);
    } else if (key == 13) {
        for (int y = 0; y < grid.getHeight(); y++) {
            for (int x = 0; x < grid.getWidth(); x++) {
                CellType t = grid.get(x, y);
                if (t == CellType::Visited || t == CellType::Path) {
                    grid.set(x, y, CellType::Empty);
                }
            }
        }

        std::vector<Cell> path;
        if (BFS::run(grid, startPos, goalPos, path)) {
            for (auto& c : path) {
                if (grid.get(c.x, c.y) != CellType::Start && grid.get(c.x, c.y) != CellType::Goal)
                    grid.set(c.x, c.y, CellType::Path);
            }
            std::cout << "Caminho encontrado!\n";
        } else {
            std::cout << "Sem caminho.\n";
        }
    } else if (key == 'r' || key == 'R') {
        for (int y = 0; y < grid.getHeight(); y++) {
            for (int x = 0; x < grid.getWidth(); x++) {
                CellType t = grid.get(x, y);
                if (t != CellType::Start && t != CellType::Goal) {
                    grid.set(x, y, CellType::Empty);
                }
            }
        }
        std::cout << "Matriz do labirinto redefinida.\n";
    }
}
