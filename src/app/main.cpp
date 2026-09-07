#include <iostream>
#include "../model/Grid/Grid.h"
#include "controller/Controller.h"
#include "view/Renderer.h"
#include "view/Camera.h"

int main(int argc, char** argv) {
    Grid grid(20, 20);

    Controller controller(grid);
    Renderer renderer(grid, controller);

    Vec3 gridCenter(grid.getWidth() / 2.0f, 0.0f, grid.getHeight() / 2.0f);
    Camera camera(gridCenter, /*radius=*/25.0f, /*azimuth=*/-90.0f, /*elevation=*/35.0f);
    controller.setCamera(&camera);
    renderer.setCamera(&camera);

    renderer.run(argc, argv);

    return 0;
}