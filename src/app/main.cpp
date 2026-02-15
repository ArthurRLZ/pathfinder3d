#include <iostream>
#include <vector>

#include "model/Grid.h"
#include "model/BFS.h"

void printGrid(const Grid& grid, const std::vector<Cell>& path) {
    int h = grid.getHeight();
    int w = grid.getWidth();

    // Marca o caminho no grid
    std::vector<std::vector<bool>> isPath(h, std::vector<bool>(w, false));
    for (const Cell& c : path) {
        isPath[c.y][c.x] = true;
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (grid.get(x, y) == CellType::Wall)
                std::cout << "# ";
            else if (grid.get(x, y) == CellType::Start)
                std::cout << "S ";
            else if (grid.get(x, y) == CellType::Goal)
                std::cout << "G ";
            else if (isPath[y][x])
                std::cout << "* ";
            else
                std::cout << ". ";
        }
        std::cout << "\n";
    }
}

int main() {
    Grid grid(20, 20);

    grid.set(1, 1, CellType::Start);    // Start
    grid.set(18, 18, CellType::Goal);  // Goal

    grid.set(5, 5, CellType::Wall);
    grid.set(5, 6, CellType::Wall);
    grid.set(5, 7, CellType::Wall);

    std::vector<Cell> path;
    bool found = BFS::run(grid, {1, 1}, {18, 18}, path);

    if (found) {
        std::cout << "Caminho encontrado!\n";
        printGrid(grid, path);
    } else {
        std::cout << "Nenhum caminho encontrado.\n";
    }

    return 0;
}