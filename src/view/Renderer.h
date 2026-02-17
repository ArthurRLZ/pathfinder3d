#pragma once

#include "model/Grid.h"
#include "model/Cell.h"

class Renderer {
public:
    Renderer(Grid& grid, Cell start, Cell goal);

    void run(int argc, char** argv);

private:
    Grid& grid;
    Cell startPos;
    Cell goalPos;

    static Renderer* instance;

    static void displayCallback();
    static void reshapeCallback(int w, int h);
    static void keyboardCallback(unsigned char key, int x, int y);

    void display();
    void reshape(int w, int h);
    void keyboard(unsigned char key, int x, int y);

    void drawCube(int x, int z, float r, float g, float b);
};
