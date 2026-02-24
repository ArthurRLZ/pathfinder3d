#include "view/Renderer.h"
#include <GL/glut.h>

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(Grid& g, Controller& c)
    : grid(g), controller(c) {}

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

    glutMouseFunc(mouseCallback);

    glutMainLoop();
}

void Renderer::displayCallback() {
    instance->display();
}

void Renderer::reshapeCallback(int w, int h) {
    instance->reshape(w, h);
}

void Renderer::keyboardCallback(unsigned char key, int, int) {
    instance->keyboard(key);
}

void Renderer::mouseCallback(int button, int state, int x, int y) {
    instance->mouse(button, state, x, y);
}

void Renderer::keyboard(unsigned char key) {
    controller.onKey(key);
    glutPostRedisplay();
}

void Renderer::drawCube(int x, int z, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glColor3f(r, g, b);
    glutSolidCube(0.95);
    glPopMatrix();
}

void Renderer::drawMiniMap() {
    int w = grid.getWidth();
    int h = grid.getHeight();

    float cellW = (float)miniMapSize / w;
    float cellH = (float)miniMapSize / h;

    // Modo 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, windowHeight, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            CellType type = grid.get(x, y);

            switch (type) {
                case CellType::Wall:   glColor3f(0.6f, 0.1f, 0.1f); break;
                case CellType::Start:  glColor3f(0.1f, 0.8f, 0.1f); break;
                case CellType::Goal:   glColor3f(0.1f, 0.1f, 0.8f); break;
                case CellType::Path:   glColor3f(0.0f, 1.0f, 1.0f); break;
                default:               glColor3f(0.8f, 0.8f, 0.8f);
            }

            float px = miniMapX + x * cellW;
            float py = miniMapY + y * cellH;

            glBegin(GL_QUADS);
            glVertex2f(px, py);
            glVertex2f(px + cellW, py);
            glVertex2f(px + cellW, py + cellH);
            glVertex2f(px, py + cellH);
            glEnd();
        }
    }

    // Borda
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(miniMapX, miniMapY);
    glVertex2f(miniMapX + miniMapSize, miniMapY);
    glVertex2f(miniMapX + miniMapSize, miniMapY + miniMapSize);
    glVertex2f(miniMapX, miniMapY + miniMapSize);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Renderer::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    drawMiniMap();

    gluLookAt(10, 18, 25,
              10, 0, 10,
              0, 1, 0);

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
    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void Renderer::mouse(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;

    if (x >= miniMapX && x <= miniMapX + miniMapSize &&
        y >= miniMapY && y <= miniMapY + miniMapSize) {

        int gridX = (x - miniMapX) * grid.getWidth() / miniMapSize;
        int gridY = (y - miniMapY) * grid.getHeight() / miniMapSize;

        controller.onMouse(gridX, gridY);
        glutPostRedisplay();
        return;
        }

    float nx = (2.0f * x) / windowWidth - 1.0f;
    float ny = 1.0f - (2.0f * y) / windowHeight;

    // câmera fixa (igual ao display)
    float camX = 10.0f, camY = 18.0f, camZ = 25.0f;

    float dirX = nx;
    float dirY = ny - 0.3f;
    float dirZ = -1.0f;

    if (dirY == 0) return;

    float t = camY / -dirY;

    float worldX = camX + dirX * t;
    float worldZ = camZ + dirZ * t;

    int gridX = static_cast<int>(worldX);
    int gridY = static_cast<int>(worldZ);

    if (grid.isInside(gridX, gridY)) {
        controller.onMouse(gridX, gridY);
        glutPostRedisplay();
    }

}
