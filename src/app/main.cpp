#include "model/Grid.h"
#include "controller/Controller.h"
#include "view/Renderer.h"

int main(int argc, char** argv) {
    Grid grid(20, 20);

    Controller controller(grid);
    Renderer renderer(grid, controller);

    renderer.run(argc, argv);
    return 0;
}