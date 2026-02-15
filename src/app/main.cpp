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
            if (grid.get(x, y) == 1)
                std::cout << "# ";
            else if (grid.get(x, y) == 2)
                std::cout << "S ";
            else if (grid.get(x, y) == 3)
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

    grid.set(1, 1, 2);    // Start
    grid.set(18, 18, 3);  // Goal

    grid.set(5, 5, 1);
    grid.set(5, 6, 1);
    grid.set(5, 7, 1);

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