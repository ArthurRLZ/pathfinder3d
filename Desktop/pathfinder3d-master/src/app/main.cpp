#include <iostream>
#include "model/Grid.h"
#include "controller/Controller.h"
#include "view/Renderer.h"
#include "view/Camera.h"

int main(int argc, char** argv) {
    Grid grid(20, 20);
    
    Controller controller(grid);
    Renderer renderer(grid, controller);
    Camera camera(10.0f, 15.0f, 25.0f, 0.0f, 1.0f, 0.0f, -90.0f, -30.0f);
    controller.setCamera(&camera);
    renderer.setCamera(&camera);

    renderer.run(argc, argv);

    return 0;
}