#include "model/Grid.h"
#include "view/Renderer.h"

int main(int argc, char** argv) {
    Grid grid(20, 20);

    Cell start(1, 1);
    Cell goal(18, 18);

    grid.set(start.x, start.y, CellType::Start);
    grid.set(goal.x, goal.y, CellType::Goal);

    grid.set(5, 5, CellType::Wall);
    grid.set(5, 6, CellType::Wall);
    grid.set(6, 5, CellType::Wall);

    Renderer renderer(grid, start, goal);
    renderer.run(argc, argv);

    return 0;
}
