#include "view/Renderer.h"
#include <GL/glut.h>
#include <cmath>

Renderer* Renderer::instance = nullptr;

Renderer::Renderer(Grid& g, Controller& c)
    : grid(g), controller(c) {}

void Renderer::setCamera(Camera* cam) {
    camera = cam;
}

void Renderer::run(int argc, char** argv) {
    instance = this;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Pathfinder 3D");

    glEnable(GL_DEPTH_TEST);

    // Ligando o motor de iluminacao do OpenGL
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // Definindo a cor da luz (branca suave)
    float ambientLight[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    float diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyboardCallback);
    glutSpecialFunc(specialKeyboardCallback);
    glutMouseFunc(mouseCallback);

    glutMainLoop();
}

void Renderer::displayCallback() { instance->display(); }
void Renderer::reshapeCallback(int w, int h) { instance->reshape(w, h); }
void Renderer::keyboardCallback(unsigned char key, int x, int y) { instance->keyboard(key); }
void Renderer::specialKeyboardCallback(int key, int x, int y) { instance->specialKeyboard(key); }
void Renderer::mouseCallback(int button, int state, int x, int y) { instance->mouse(button, state, x, y); }

void Renderer::keyboard(unsigned char key) {
    controller.onKey(key);
    glutPostRedisplay();
}

void Renderer::specialKeyboard(int key) {
    controller.onSpecialKey(key);
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

    glColor3f(1.0f, 1.0f, 1.0f); 
    const char* legend[] = {
        "Controles:",
        "S: Start | G: Goal",
        "W: Parede | E: Borracha",
        "1: BFS | 2: A-Star",
        "ENTER: Rodar | R: Reset",
        "Setas: Andar | A/D: Girar"
    };

    int textY = miniMapY + miniMapSize + 25;
    for (int i = 0; i < 6; i++) {
        glRasterPos2f(miniMapX, textY + (i * 20));
        for (const char* c = legend[i]; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Renderer::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (camera) {
        Vec3 target = camera->GetTarget();
        gluLookAt(camera->Position.x, camera->Position.y, camera->Position.z,
                  target.x, target.y, target.z,
                  camera->Up.x, camera->Up.y, camera->Up.z);
    }

    // Posiciona a luz no meio do cenario, no alto
    float lightPos[] = { 10.0f, 15.0f, 10.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    for (int y = 0; y < grid.getHeight(); y++) {
        for (int x = 0; x < grid.getWidth(); x++) {
            CellType type = grid.get(x, y);
            switch (type) {
                case CellType::Wall:   drawCube(x, y, 0.8f, 0.2f, 0.2f); break;
                case CellType::Start:  drawCube(x, y, 0.2f, 0.8f, 0.2f); break;
                case CellType::Goal:   drawCube(x, y, 0.2f, 0.2f, 0.8f); break;
                case CellType::Path:   drawCube(x, y, 0.0f, 1.0f, 1.0f); break;
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

    // Desliga a luz rapidinho para a interface 2D nao ficar escura
    glDisable(GL_LIGHTING);
    drawMiniMap();
    glEnable(GL_LIGHTING);

    glutSwapBuffers();
}

void Renderer::reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void Renderer::mouse(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

    if (x >= miniMapX && x <= miniMapX + miniMapSize &&
        y >= miniMapY && y <= miniMapY + miniMapSize) {
        int gridX = (x - miniMapX) * grid.getWidth() / miniMapSize;
        int gridY = (y - miniMapY) * grid.getHeight() / miniMapSize;
        controller.onMouse(gridX, gridY);
        glutPostRedisplay();
        return;
    }

    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    GLfloat winX, winY;
    GLdouble posX, posY, posZ, dirX, dirY, dirZ;

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;

    gluUnProject(winX, winY, 0.0, modelview, projection, viewport, &posX, &posY, &posZ);
    gluUnProject(winX, winY, 1.0, modelview, projection, viewport, &dirX, &dirY, &dirZ);

    float rDx = dirX - posX, rDy = dirY - posY, rDz = dirZ - posZ;
    if (std::abs(rDy) > 0.0001f) {
        float t = -posY / rDy;
        int gx = static_cast<int>(std::round(posX + rDx * t));
        int gy = static_cast<int>(std::round(posZ + rDz * t));
        controller.onMouse(gx, gy);
    }
    glutPostRedisplay();
}