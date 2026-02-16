#include <GL/glut.h>
#include <iostream>
#include <vector>
#include "../model/Grid.h"
#include "../model/BFS.h"

Grid grid(20, 20);
int width = 800;
int height = 600;

Cell startPos(1, 1);
Cell goalPos(18, 18);

void drawCube(int x, int z, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x * 1.0f, 0.0f, z * 1.0f);
    glColor3f(r, g, b);
    glutSolidCube(0.95);
    glColor3f(0.0f, 0.0f, 0.0f);
    glutWireCube(0.95);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(10.0, 18.0, 25.0,
              10.0, 0.0, 10.0,
              0.0, 1.0, 0.0);

    for (int z = 0; z < grid.getHeight(); z++) {
        for (int x = 0; x < grid.getWidth(); x++) {
            CellType type = grid.get(x, z);

            if (type == CellType::Wall) {
                drawCube(x, z, 0.8f, 0.2f, 0.2f);
            }
            else if (type == CellType::Start) {
                drawCube(x, z, 0.2f, 0.8f, 0.2f);
            }
            else if (type == CellType::Goal) {
                drawCube(x, z, 0.2f, 0.2f, 0.8f);
            }
            else if (type == CellType::Visited) {
                glPushMatrix();
                glTranslatef(x * 1.0f, -0.4f, z * 1.0f);
                glColor3f(1.0f, 1.0f, 0.0f);
                glScalef(0.9f, 0.1f, 0.9f);
                glutSolidCube(1.0);
                glPopMatrix();
            }
            else if (type == CellType::Path) {
                glPushMatrix();
                glTranslatef(x * 1.0f, -0.4f, z * 1.0f);
                glColor3f(0.0f, 1.0f, 1.0f);
                glScalef(0.9f, 0.1f, 0.9f);
                glutSolidCube(1.0);
                glPopMatrix();
            }
            else {
                glPushMatrix();
                glTranslatef(x * 1.0f, -0.5f, z * 1.0f);
                glColor3f(0.8f, 0.8f, 0.8f);
                glScalef(0.95f, 0.1f, 0.95f);
                glutSolidCube(1.0);
                glPopMatrix();
            }
        }
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    width = w; height = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 13) {
        std::vector<Cell> path;
        std::cout << "Iniciando BFS..." << std::endl;
        if (BFS::run(grid, startPos, goalPos, path)) {
            std::cout << "Caminho encontrado com " << path.size() << " passos!" << std::endl;
            for(auto p : path) {
                if(grid.get(p.x, p.y) != CellType::Start && grid.get(p.x, p.y) != CellType::Goal)
                    grid.set(p.x, p.y, CellType::Path);
            }
        } else {
            std::cout << "Impossivel chegar no destino." << std::endl;
        }
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    grid.set(startPos.x, startPos.y, CellType::Start);
    grid.set(goalPos.x, goalPos.y, CellType::Goal);

    grid.set(5, 5, CellType::Wall);
    grid.set(5, 6, CellType::Wall);
    grid.set(6, 5, CellType::Wall);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Pathfinder 3D - UFAPE");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    std::cout << "Janela Aberta! Aperte ENTER na janela para rodar o BFS." << std::endl;
    glutMainLoop();
    return 0;
}