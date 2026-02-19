#include "Renderer.h"
#include "model/BFS.h"
#include <GL/glut.h>
#include <iostream>

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(Grid& g, Cell start, Cell goal)
    : grid(g), startPos(start), goalPos(goal) {}

void Renderer::run(int argc, char** argv) {
    instance = this;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Pathfinder 3D");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyboardCallback);

    std::cout << "Aperte ENTER para rodar o BFS\n";
    std::cout << "Aperte R para limpar o labirinto\n";
    glutMainLoop();
}

void Renderer::displayCallback() { instance->display(); }
void Renderer::reshapeCallback(int w, int h) { instance->reshape(w, h); }
void Renderer::keyboardCallback(unsigned char key, int x, int y) {
    instance->keyboard(key, x, y);
}

void Renderer::drawCube(int x, int z, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glColor3f(r, g, b);
    glutSolidCube(0.95);
    glPopMatrix();
}

void Renderer::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(10, 18, 25, 10, 0, 10, 0, 1, 0);

    for (int y = 0; y < grid.getHeight(); y++) {
        for (int x = 0; x < grid.getWidth(); x++) {
            CellType type = grid.get(x, y);
            
            switch (type) {
                case CellType::Wall:
                    drawCube(x, y, 0.8f, 0.2f, 0.2f);
                    break;
                case CellType::Start:
                    drawCube(x, y, 0.2f, 0.8f, 0.2f);
                    break;
                case CellType::Goal:
                    drawCube(x, y, 0.2f, 0.2f, 0.8f);
                    break;
                case CellType::Path:
                    drawCube(x, y, 0.0f, 1.0f, 1.0f); 
                    break;
                case CellType::Visited:

                    glPushMatrix();
                    glTranslatef(x, -0.4f, y);
                    glColor3f(1.0f, 1.0f, 0.0f);
                    glScalef(0.95f, 0.1f, 0.95f);
                    glutSolidCube(1.0);
                    glPopMatrix();
                    break;
                default:
                    glPushMatrix();
                    glTranslatef(x, -0.5f, y);
                    glColor3f(0.8f, 0.8f, 0.8f);
                    glScalef(0.95f, 0.1f, 0.95f);
                    glutSolidCube(1.0);
                    glPopMatrix();
            }
        }
    }

    glutSwapBuffers();
}
void Renderer::reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}
void Renderer::keyboard(unsigned char key, int, int) {
    if (key == 13) { 
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
        glutPostRedisplay();
    }
    else if (key == 'r' || key == 'R') {
        for (int y = 0; y < grid.getHeight(); y++) {
            for (int x = 0; x < grid.getWidth(); x++) {
                CellType t = grid.get(x, y);
                if (t != CellType::Start && t != CellType::Goal) {
                    grid.set(x, y, CellType::Empty);
                }
            }
        }
        std::cout << "Matriz do labirinto redefinida.\n";
        glutPostRedisplay();
    }
}