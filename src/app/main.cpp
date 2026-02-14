#include <iostream>
#include <vector>

#include "model/Grid.h"
#include "model/BFS.h"

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
    } else {
        std::cout << "Nenhum caminho encontrado.\n";
    }

    return 0;
}