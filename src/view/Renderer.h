#pragma once

#include "model/Grid.h"
#include "model/Cell.h"
#include "controller/Controller.h"

class Renderer {
public:
    Renderer(Grid& grid, Controller& controller);

    void run(int argc, char** argv);

private:
    Grid& grid;
    Controller& controller;

    int windowWidth = 800;
    int windowHeight = 600;

    static Renderer* instance;

    static void displayCallback();
    static void reshapeCallback(int w, int h);
    static void keyboardCallback(unsigned char key, int x, int y);
    static void mouseCallback(int button, int state, int x, int y);

    void display();
    void reshape(int w, int h);
    void keyboard(unsigned char key);
    void mouse(int button, int state, int x, int y);

    void drawCube(int x, int z, float r, float g, float b);
};