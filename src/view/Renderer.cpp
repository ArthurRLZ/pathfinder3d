#include "view/Renderer.h"
#include "../model/Algorithms/AlgorithmType.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdio>

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

void Renderer::drawButton(int x, int y, int w, int h, const char* label, bool highlighted) {
    glColor3f(highlighted ? 0.2f : 0.3f, highlighted ? 0.6f : 0.3f, highlighted ? 0.9f : 0.33f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x + 8, y + h - 10);
    for (const char* c = label; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void Renderer::drawMenu() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, windowHeight, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Fundo cobrindo a janela inteira, para não ver o cenário 3D atrás.
    glColor3f(0.1f, 0.1f, 0.13f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    const char* title = "Pathfinder3D - Novo Grid";
    glRasterPos2f(40, 50);
    for (const char* c = title; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    int gridSize = controller.getPendingGridSize();
    char sizeText[64];
    snprintf(sizeText, sizeof(sizeText), "Tamanho do grid: %d x %d", gridSize, gridSize);
    glRasterPos2f(40, 100);
    for (const char* c = sizeText; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    menuMinusX = 320; menuMinusY = 82;
    menuPlusX = 370;  menuPlusY = 82;
    drawButton(menuMinusX, menuMinusY, menuMinusW, menuMinusH, "-", false);
    drawButton(menuPlusX, menuPlusY, menuPlusW, menuPlusH, "+", false);

    glColor3f(1.0f, 1.0f, 1.0f);
    const char* algoLabel = "Algoritmos para comparar (tecla C na simulacao):";
    glRasterPos2f(40, 150);
    for (const char* c = algoLabel; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);

    menuAlgoX = 40;
    menuAlgoY = 170;
    for (int i = 0; i < kAlgorithmCount; i++) {
        AlgorithmType type = kAllAlgorithmTypes[i];
        bool selected = controller.isAlgorithmSelected(type);

        char label[32];
        snprintf(label, sizeof(label), "[%s] %s", selected ? "x" : " ", algorithmName(type));
        drawButton(menuAlgoX, menuAlgoY + i * menuAlgoSpacing, menuAlgoW, menuAlgoH, label, selected);
    }

    menuConfirmX = menuAlgoX;
    menuConfirmY = menuAlgoY + kAlgorithmCount * menuAlgoSpacing + 20;
    drawButton(menuConfirmX, menuConfirmY, menuConfirmW, menuConfirmH, "Confirmar (Enter)", true);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Renderer::drawResults() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, windowHeight, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(0.1f, 0.1f, 0.13f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    const char* title = "Resultados da comparacao";
    glRasterPos2f(40, 50);
    for (const char* c = title; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    const char* header = "Algoritmo      Achou?  Passos  Visitadas  Tempo(ms)";
    glRasterPos2f(40, 90);
    for (const char* c = header; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);

    const auto& results = controller.getComparisonResults();
    int rowY = 115;
    for (const auto& row : results) {
        char line[128];
        snprintf(line, sizeof(line), "%-14s %-7s %-7d %-10d %.3f",
                 algorithmName(row.type),
                 row.found ? "Sim" : "Nao",
                 row.pathLength,
                 row.visitedCount,
                 row.elapsedMs);
        glRasterPos2f(40, rowY);
        for (const char* c = line; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        rowY += 22;
    }

    resultsBackX = 40;
    resultsBackY = rowY + 20;
    drawButton(resultsBackX, resultsBackY, resultsBackW, resultsBackH, "Voltar (qualquer tecla)", true);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
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
                case CellType::Wall:    glColor3f(0.6f, 0.1f, 0.1f); break;
                case CellType::Start:   glColor3f(0.1f, 0.8f, 0.1f); break;
                case CellType::Goal:    glColor3f(0.1f, 0.1f, 0.8f); break;
                case CellType::Path:    glColor3f(0.0f, 1.0f, 1.0f); break;
                case CellType::Visited: glColor3f(0.9f, 0.6f, 0.1f); break;
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
        "3: DFS | 4: Dijkstra",
        "ENTER: Rodar | R: Reset",
        "C: Comparar algoritmos",
        "Setas/A-D: Girar camera",
        "Setas cima/baixo: Inclinar"
    };

    int textY = miniMapY + miniMapSize + 25;
    int legendLines = sizeof(legend) / sizeof(legend[0]);
    for (int i = 0; i < legendLines; i++) {
        glRasterPos2f(miniMapX, textY + (i * 20));
        for (const char* c = legend[i]; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }

    // Botão para alternar entre a órbita livre (3D) e a vista de topo (2D).
    // Não é um widget nativo do GLUT: é um retângulo desenhado neste mesmo
    // overlay 2D, cujo clique é detectado "na mão" em Renderer::mouse().
    topViewButtonX = miniMapX;
    topViewButtonY = textY + (legendLines * 20) + 15;

    bool topView = controller.isTopView();
    glColor3f(topView ? 0.2f : 0.3f, topView ? 0.6f : 0.3f, topView ? 0.9f : 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(topViewButtonX, topViewButtonY);
    glVertex2f(topViewButtonX + topViewButtonW, topViewButtonY);
    glVertex2f(topViewButtonX + topViewButtonW, topViewButtonY + topViewButtonH);
    glVertex2f(topViewButtonX, topViewButtonY + topViewButtonH);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    const char* buttonLabel = topView ? "Vista 2D (clique: 3D)" : "Vista 3D (clique: 2D)";
    glRasterPos2f(topViewButtonX + 10, topViewButtonY + 20);
    for (const char* c = buttonLabel; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Renderer::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    AppState state = controller.getAppState();

    if (state == AppState::Menu) {
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        drawMenu();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glutSwapBuffers();
        return;
    }

    if (state == AppState::Results) {
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        drawResults();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glutSwapBuffers();
        return;
    }

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
                case CellType::Visited:
                    // Célula já explorada pelo algoritmo: continua "piso" (não é
                    // obstáculo), só que pintada de laranja para mostrar o rastro
                    // da busca em andamento.
                    glPushMatrix();
                    glTranslatef(x, -0.5f, y);
                    glColor3f(0.9f, 0.6f, 0.1f);
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

    // Desliga luz e teste de profundidade para a interface 2D: sem isso, o
    // teste de profundidade (GL_LESS) descarta qualquer desenho posterior
    // que sobreponha um pixel já escrito na mesma profundidade (z=0) —
    // exatamente o bug que deixava o menu com a tela preta e vazia.
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    drawMiniMap();
    glEnable(GL_DEPTH_TEST);
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

    AppState appState = controller.getAppState();

    if (appState == AppState::Menu) {
        if (x >= menuMinusX && x <= menuMinusX + menuMinusW &&
            y >= menuMinusY && y <= menuMinusY + menuMinusH) {
            controller.decreaseGridSize();
        } else if (x >= menuPlusX && x <= menuPlusX + menuPlusW &&
                   y >= menuPlusY && y <= menuPlusY + menuPlusH) {
            controller.increaseGridSize();
        } else if (x >= menuAlgoX && x <= menuAlgoX + menuAlgoW && y >= menuAlgoY) {
            int rel = y - menuAlgoY;
            int index = rel / menuAlgoSpacing;
            if (index >= 0 && index < kAlgorithmCount && (rel % menuAlgoSpacing) <= menuAlgoH) {
                controller.toggleAlgorithmSelected(kAllAlgorithmTypes[index]);
            }
        } else if (x >= menuConfirmX && x <= menuConfirmX + menuConfirmW &&
                   y >= menuConfirmY && y <= menuConfirmY + menuConfirmH) {
            controller.confirmMenu();
        }
        glutPostRedisplay();
        return;
    }

    if (appState == AppState::Results) {
        if (x >= resultsBackX && x <= resultsBackX + resultsBackW &&
            y >= resultsBackY && y <= resultsBackY + resultsBackH) {
            controller.backToSimulation();
        }
        glutPostRedisplay();
        return;
    }

    if (x >= topViewButtonX && x <= topViewButtonX + topViewButtonW &&
        y >= topViewButtonY && y <= topViewButtonY + topViewButtonH) {
        controller.toggleTopView();
        glutPostRedisplay();
        return;
    }

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