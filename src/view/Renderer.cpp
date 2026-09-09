#include "view/Renderer.h"
#include "../model/Algorithms/AlgorithmType.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <algorithm>

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

int Renderer::textWidth(void* font, const char* text) {
    int w = 0;
    for (const char* c = text; *c != '\0'; c++) {
        w += glutBitmapWidth(font, *c);
    }
    return w;
}

void Renderer::drawButton(int x, int y, int w, int h, const char* label, bool highlighted, void* font) {
    // Borda sutil ao redor do botão: dá uma sensação de profundidade/contorno
    // em vez do retângulo plano "cru" de antes.
    glColor3f(0.04f, 0.04f, 0.06f);
    glBegin(GL_QUADS);
    glVertex2f(x - 2, y - 2);
    glVertex2f(x + w + 2, y - 2);
    glVertex2f(x + w + 2, y + h + 2);
    glVertex2f(x - 2, y + h + 2);
    glEnd();

    if (highlighted) glColor3f(0.24f, 0.55f, 0.92f);
    else glColor3f(0.22f, 0.23f, 0.27f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();

    // Friso mais claro na borda superior, imitando um leve efeito "elevado"
    // (bevel) comum em botões de UI flat modernas.
    if (highlighted) glColor3f(0.45f, 0.72f, 1.0f);
    else glColor3f(0.34f, 0.36f, 0.40f);
    glBegin(GL_LINES);
    glVertex2f(x, y + 1);
    glVertex2f(x + w, y + 1);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    int tw = textWidth(font, label);
    int fontHeight = (font == GLUT_BITMAP_HELVETICA_18) ? 18 : 12;
    float textX = x + (w - tw) / 2.0f;
    float textY = y + h / 2.0f + fontHeight / 2.0f - 3.0f;
    glRasterPos2f(textX, textY);
    for (const char* c = label; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
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
    glColor3f(0.09f, 0.09f, 0.12f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    // Painel central: tudo é posicionado a partir de panelX (calculado pra
    // ficar sempre centralizado na janela, mesmo se ela for redimensionada).
    int panelWidth = menuAlgoW;
    int panelX = (windowWidth - panelWidth) / 2;

    // Altura total do conteúdo, calculada de antemão só para poder
    // centralizar o bloco inteiro verticalmente também.
    int contentHeight = 40  /*título*/ + 30 /*subtítulo*/ + 55 /*seletor de tamanho*/
                       + 30 /*rótulo algoritmos*/ + kAlgorithmCount * menuAlgoSpacing
                       + 20 + menuAutoWallsH + 25 + menuConfirmH;
    int y = std::max(30, (windowHeight - contentHeight) / 2);

    glColor3f(1.0f, 1.0f, 1.0f);
    {
        const char* title = "Pathfinder3D";
        int tw = textWidth(GLUT_BITMAP_TIMES_ROMAN_24, title);
        glRasterPos2f(panelX + (panelWidth - tw) / 2.0f, y);
        for (const char* c = title; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    y += 40;

    glColor3f(0.75f, 0.75f, 0.8f);
    {
        const char* subtitle = "Configurar novo grid";
        int tw = textWidth(GLUT_BITMAP_HELVETICA_18, subtitle);
        glRasterPos2f(panelX + (panelWidth - tw) / 2.0f, y);
        for (const char* c = subtitle; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    y += 45;

    // Seletor de tamanho: "-" [ NxN ] "+", centralizado como uma linha só.
    int gridSize = controller.getPendingGridSize();
    char sizeText[32];
    snprintf(sizeText, sizeof(sizeText), "%d x %d", gridSize, gridSize);
    int sizeTextW = textWidth(GLUT_BITMAP_HELVETICA_18, sizeText);
    int sizeLabelW = 140; // espaço reservado pro número, entre os botões -/+
    int rowW = menuMinusW + 12 + sizeLabelW + 12 + menuPlusW;
    int rowX = panelX + (panelWidth - rowW) / 2;

    menuMinusX = rowX;
    menuMinusY = y;
    drawButton(menuMinusX, menuMinusY, menuMinusW, menuMinusH, "-", false, GLUT_BITMAP_HELVETICA_18);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(rowX + menuMinusW + 12 + (sizeLabelW - sizeTextW) / 2.0f, y + menuMinusH / 2.0f + 6);
    for (const char* c = sizeText; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    menuPlusX = rowX + menuMinusW + 12 + sizeLabelW + 12;
    menuPlusY = y;
    drawButton(menuPlusX, menuPlusY, menuPlusW, menuPlusH, "+", false, GLUT_BITMAP_HELVETICA_18);

    y += menuMinusH + 25;

    glColor3f(0.75f, 0.75f, 0.8f);
    {
        const char* algoLabel = "Algoritmos para comparar (tecla C na simulacao)";
        int tw = textWidth(GLUT_BITMAP_HELVETICA_12, algoLabel);
        glRasterPos2f(panelX + (panelWidth - tw) / 2.0f, y);
        for (const char* c = algoLabel; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    y += 20;

    menuAlgoX = panelX;
    menuAlgoY = y;
    for (int i = 0; i < kAlgorithmCount; i++) {
        AlgorithmType type = kAllAlgorithmTypes[i];
        bool selected = controller.isAlgorithmSelected(type);

        char label[32];
        snprintf(label, sizeof(label), "[%s]  %s", selected ? "x" : " ", algorithmName(type));
        drawButton(menuAlgoX, menuAlgoY + i * menuAlgoSpacing, menuAlgoW, menuAlgoH, label, selected, GLUT_BITMAP_HELVETICA_18);
    }
    y = menuAlgoY + kAlgorithmCount * menuAlgoSpacing + 10;

    menuAutoWallsX = panelX;
    menuAutoWallsY = y;
    {
        bool autoWalls = controller.isAutoWallsEnabled();
        char label[48];
        snprintf(label, sizeof(label), "[%s]  Gerar paredes automaticamente", autoWalls ? "x" : " ");
        drawButton(menuAutoWallsX, menuAutoWallsY, menuAutoWallsW, menuAutoWallsH, label, autoWalls, GLUT_BITMAP_HELVETICA_18);
    }
    y += menuAutoWallsH + 25;

    menuConfirmX = panelX;
    menuConfirmY = y;
    drawButton(menuConfirmX, menuConfirmY, menuConfirmW, menuConfirmH, "Confirmar (Enter)", true, GLUT_BITMAP_HELVETICA_18);

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

    glColor3f(0.09f, 0.09f, 0.12f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();

    const auto& results = controller.getComparisonResults();

    // Larguras de cada coluna da tabela. A última ("Caminho") reserva espaço
    // para a miniatura da grid com Visited/Path pintados.
    const int colAlgoW = 120;
    const int colFoundW = 70;
    const int colStepsW = 70;
    const int colVisitedW = 90;
    const int colTimeW = 95;
    const int thumbSize = 64;
    const int colMapW = thumbSize + 24;

    int colW[6] = { colAlgoW, colFoundW, colStepsW, colVisitedW, colTimeW, colMapW };
    const char* headers[6] = { "Algoritmo", "Achou?", "Passos", "Visitadas", "Tempo (ms)", "Caminho" };

    int tableWidth = colAlgoW + colFoundW + colStepsW + colVisitedW + colTimeW + colMapW;
    int headerH = 32;
    int rowH = thumbSize + 20;
    int rowCount = static_cast<int>(results.size());

    int tableX = (windowWidth - tableWidth) / 2;
    int buttonsRowWidth = resultsBackW + 20 + resultsMenuW;

    int contentHeight = 55 /*título*/ + headerH + rowCount * rowH + 30 + resultsBackH;
    int y = std::max(20, (windowHeight - contentHeight) / 2);

    glColor3f(1.0f, 1.0f, 1.0f);
    {
        const char* title = "Resultados da comparacao";
        int tw = textWidth(GLUT_BITMAP_TIMES_ROMAN_24, title);
        glRasterPos2f((windowWidth - tw) / 2.0f, y);
        for (const char* c = title; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    y += 45;

    int tableTop = y;
    int colX[6];
    colX[0] = tableX;
    for (int i = 1; i < 6; i++) colX[i] = colX[i - 1] + colW[i - 1];

    // Fundo do cabeçalho + rótulos centralizados em cada coluna.
    glColor3f(0.16f, 0.18f, 0.22f);
    glBegin(GL_QUADS);
    glVertex2f(tableX, y);
    glVertex2f(tableX + tableWidth, y);
    glVertex2f(tableX + tableWidth, y + headerH);
    glVertex2f(tableX, y + headerH);
    glEnd();

    glColor3f(0.8f, 0.82f, 0.88f);
    for (int i = 0; i < 6; i++) {
        int tw = textWidth(GLUT_BITMAP_HELVETICA_12, headers[i]);
        glRasterPos2f(colX[i] + (colW[i] - tw) / 2.0f, y + headerH / 2.0f + 4);
        for (const char* c = headers[i]; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    y += headerH;

    // Linhas de dados: fundo listrado (facilita ler a linha certa), texto
    // centralizado em cada coluna, e a miniatura do caminho na última.
    for (int r = 0; r < rowCount; r++) {
        const auto& row = results[r];

        if (r % 2 == 0) glColor3f(0.14f, 0.15f, 0.18f);
        else glColor3f(0.10f, 0.11f, 0.14f);
        glBegin(GL_QUADS);
        glVertex2f(tableX, y);
        glVertex2f(tableX + tableWidth, y);
        glVertex2f(tableX + tableWidth, y + rowH);
        glVertex2f(tableX, y + rowH);
        glEnd();

        char buf[5][32];
        snprintf(buf[0], sizeof(buf[0]), "%s", algorithmName(row.type));
        snprintf(buf[1], sizeof(buf[1]), "%s", row.found ? "Sim" : "Nao");
        snprintf(buf[2], sizeof(buf[2]), "%d", row.pathLength);
        snprintf(buf[3], sizeof(buf[3]), "%d", row.visitedCount);
        snprintf(buf[4], sizeof(buf[4]), "%.3f", row.elapsedMs);

        glColor3f(1.0f, 1.0f, 1.0f);
        for (int i = 0; i < 5; i++) {
            int tw = textWidth(GLUT_BITMAP_HELVETICA_12, buf[i]);
            glRasterPos2f(colX[i] + (colW[i] - tw) / 2.0f, y + rowH / 2.0f + 4);
            for (const char* c = buf[i]; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }

        float thumbX = colX[5] + (colMapW - thumbSize) / 2.0f;
        float thumbY = y + (rowH - thumbSize) / 2.0f;
        drawGridThumbnail(row.snapshot, thumbX, thumbY, static_cast<float>(thumbSize));

        y += rowH;
    }

    int tableBottom = tableTop + headerH + rowCount * rowH;

    // Bordas da tabela: verticais entre colunas, horizontais entre linhas —
    // isso é o que faz o resultado parecer uma tabela de verdade, em vez de
    // texto alinhado por espaços.
    glColor3f(0.32f, 0.34f, 0.4f);
    glBegin(GL_LINES);
    for (int i = 0; i <= 6; i++) {
        int lx = (i == 6) ? tableX + tableWidth : colX[i];
        glVertex2f(lx, tableTop);
        glVertex2f(lx, tableBottom);
    }
    for (int i = 0; i <= rowCount + 1; i++) {
        int ly = (i == 0) ? tableTop : (i == 1) ? tableTop + headerH : tableTop + headerH + (i - 1) * rowH;
        glVertex2f(tableX, ly);
        glVertex2f(tableX + tableWidth, ly);
    }
    glEnd();

    y = tableBottom + 30;

    int rowX = (windowWidth - buttonsRowWidth) / 2;
    resultsBackX = rowX;
    resultsBackY = y;
    drawButton(resultsBackX, resultsBackY, resultsBackW, resultsBackH, "Voltar (tecla)", true, GLUT_BITMAP_HELVETICA_18);

    resultsMenuX = rowX + resultsBackW + 20;
    resultsMenuY = y;
    drawButton(resultsMenuX, resultsMenuY, resultsMenuW, resultsMenuH, "Voltar ao menu", false, GLUT_BITMAP_HELVETICA_18);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Renderer::drawGridThumbnail(const Grid& g, float x, float y, float size) {
    int w = g.getWidth();
    int h = g.getHeight();
    float cell = size / static_cast<float>(std::max(w, h));

    // Fundo (equivalente ao piso cinza-claro das células Empty na cena 3D
    // e no minimapa).
    glColor3f(0.82f, 0.82f, 0.82f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w * cell, y);
    glVertex2f(x + w * cell, y + h * cell);
    glVertex2f(x, y + h * cell);
    glEnd();

    // Mesmas cores do minimapa (drawMiniMap), célula por célula.
    for (int gy = 0; gy < h; gy++) {
        for (int gx = 0; gx < w; gx++) {
            CellType t = g.get(gx, gy);
            if (t == CellType::Empty) continue;

            switch (t) {
                case CellType::Wall:    glColor3f(0.6f, 0.1f, 0.1f); break;
                case CellType::Start:   glColor3f(0.1f, 0.8f, 0.1f); break;
                case CellType::Goal:    glColor3f(0.1f, 0.1f, 0.8f); break;
                case CellType::Path:    glColor3f(0.0f, 1.0f, 1.0f); break;
                case CellType::Visited: glColor3f(0.9f, 0.6f, 0.1f); break;
                default: continue;
            }

            float cx = x + gx * cell;
            float cy = y + gy * cell;
            glBegin(GL_QUADS);
            glVertex2f(cx, cy);
            glVertex2f(cx + cell, cy);
            glVertex2f(cx + cell, cy + cell);
            glVertex2f(cx, cy + cell);
            glEnd();
        }
    }

    glColor3f(0.35f, 0.35f, 0.4f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + w * cell, y);
    glVertex2f(x + w * cell, y + h * cell);
    glVertex2f(x, y + h * cell);
    glEnd();
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

    // Botão para alternar entre a órbita livre (3D) e a vista de topo (2D),
    // e botão para voltar ao menu inicial. Não são widgets nativos do GLUT:
    // são retângulos desenhados neste mesmo overlay 2D, cujo clique é
    // detectado "na mão" em Renderer::mouse().
    topViewButtonX = miniMapX;
    topViewButtonY = textY + (legendLines * 20) + 15;
    bool topView = controller.isTopView();
    const char* buttonLabel = topView ? "Vista 2D (clique: 3D)" : "Vista 3D (clique: 2D)";
    drawButton(topViewButtonX, topViewButtonY, topViewButtonW, topViewButtonH, buttonLabel, topView, GLUT_BITMAP_HELVETICA_12);

    simMenuButtonX = miniMapX;
    simMenuButtonY = topViewButtonY + topViewButtonH + 12;
    drawButton(simMenuButtonX, simMenuButtonY, simMenuButtonW, simMenuButtonH, "Voltar ao menu", false, GLUT_BITMAP_HELVETICA_12);

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
        } else if (x >= menuAlgoX && x <= menuAlgoX + menuAlgoW &&
                   y >= menuAlgoY && y < menuAlgoY + kAlgorithmCount * menuAlgoSpacing) {
            int rel = y - menuAlgoY;
            int index = rel / menuAlgoSpacing;
            if (index >= 0 && index < kAlgorithmCount && (rel % menuAlgoSpacing) <= menuAlgoH) {
                controller.toggleAlgorithmSelected(kAllAlgorithmTypes[index]);
            }
        } else if (x >= menuAutoWallsX && x <= menuAutoWallsX + menuAutoWallsW &&
                   y >= menuAutoWallsY && y <= menuAutoWallsY + menuAutoWallsH) {
            controller.toggleAutoWalls();
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
        } else if (x >= resultsMenuX && x <= resultsMenuX + resultsMenuW &&
                   y >= resultsMenuY && y <= resultsMenuY + resultsMenuH) {
            controller.backToMenu();
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

    if (x >= simMenuButtonX && x <= simMenuButtonX + simMenuButtonW &&
        y >= simMenuButtonY && y <= simMenuButtonY + simMenuButtonH) {
        controller.backToMenu();
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