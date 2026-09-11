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
    glutInitWindowSize(1920, 1080);
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
    glutSolidCube(0.93);
    // Borda escura desenhada como wireframe ligeiramente maior que o cubo
    // sólido, separando visualmente células adjacentes sem precisar de
    // textura (estilo "flat shaded + outline").
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glColor3f(0.024f, 0.031f, 0.059f); // slate-950
    glutWireCube(0.96);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

int Renderer::textWidth(void* font, const char* text) {
    int w = 0;
    for (const char* c = text; *c != '\0'; c++) {
        w += glutBitmapWidth(font, *c);
    }
    return w;
}

void Renderer::drawRoundedRect(float x, float y, float w, float h, float radius, float r, float g, float b, float a) {
    if (radius > w / 2.0f) radius = w / 2.0f;
    if (radius > h / 2.0f) radius = h / 2.0f;
    if (radius < 0.0f) radius = 0.0f;

    const int segs = 8; // segmentos por canto arredondado
    const float pi = 3.14159265f;

    struct Corner { float cx, cy, fromDeg, toDeg; };
    Corner corners[4] = {
        { x + w - radius, y + radius,       270.0f, 360.0f }, // topo-direita
        { x + w - radius, y + h - radius,     0.0f,  90.0f }, // baixo-direita
        { x + radius,     y + h - radius,    90.0f, 180.0f }, // baixo-esquerda
        { x + radius,     y + radius,       180.0f, 270.0f }, // topo-esquerda
    };

    glColor4f(r, g, b, a);
    glBegin(GL_POLYGON);
    for (auto& c : corners) {
        for (int i = 0; i <= segs; i++) {
            float t = c.fromDeg + (c.toDeg - c.fromDeg) * (i / (float)segs);
            float rad = t * pi / 180.0f;
            glVertex2f(c.cx + cosf(rad) * radius, c.cy + sinf(rad) * radius);
        }
    }
    glEnd();
}

void Renderer::drawToggleSwitch(float x, float y, float w, float h, bool on) {
    float radius = h / 2.0f;

    // Paleta slate & indigo: ON em indigo-500 (acento primário),
    // OFF em slate-600 (neutro escuro, mas ainda distinto do fundo slate-800).
    if (on) drawRoundedRect(x, y, w, h, radius, 0.388f, 0.396f, 0.945f, 1.0f);   // indigo-500
    else    drawRoundedRect(x, y, w, h, radius, 0.278f, 0.333f, 0.412f, 1.0f);   // slate-600

    // Bolinha (knob): fica encostada na esquerda quando desligado, na
    // direita quando ligado — a metáfora visual padrão de toggle switch.
    float knobRadius = radius - 3.0f;
    float knobCx = on ? (x + w - radius) : (x + radius);
    float knobCy = y + radius;

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(knobCx, knobCy);
    const int segs = 20;
    for (int i = 0; i <= segs; i++) {
        float a = i * 2.0f * 3.14159265f / segs;
        glVertex2f(knobCx + cosf(a) * knobRadius, knobCy + sinf(a) * knobRadius);
    }
    glEnd();
}

void Renderer::drawButton(int x, int y, int w, int h, const char* label, bool highlighted, void* font) {
    float radius = h / 3.0f;
    if (radius > 14.0f) radius = 14.0f;

    // Halo indigo suave atrás do botão destacado, puxando o olhar pra ação
    // primária (alpha baixo pra não virar neon).
    if (highlighted) drawRoundedRect(x - 4, y - 4, w + 8, h + 8, radius + 4, 0.388f, 0.396f, 0.945f, 0.18f);

    // Sombra deslocada pra baixo/direita, dando sensação de elevação.
    drawRoundedRect(x + 2, y + 4, w, h, radius, 0.0f, 0.0f, 0.0f, 0.30f);

    // Corpo do botão: indigo-600 (destacado/primário) ou slate-700 (normal).
    if (highlighted) drawRoundedRect(x, y, w, h, radius, 0.310f, 0.275f, 0.898f, 1.0f); // indigo-600
    else             drawRoundedRect(x, y, w, h, radius, 0.200f, 0.255f, 0.333f, 1.0f); // slate-700

    // Gradiente sutil: metade de cima num tom mais claro ("glassy top").
    if (highlighted) drawRoundedRect(x + 1.5f, y + 1.5f, w - 3.0f, h / 2.0f - 1.5f, radius * 0.7f, 1.0f, 1.0f, 1.0f, 0.14f);
    else             drawRoundedRect(x + 1.5f, y + 1.5f, w - 3.0f, h / 2.0f - 1.5f, radius * 0.7f, 1.0f, 1.0f, 1.0f, 0.06f);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
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

    // Fundo com gradiente vertical slate-900 (topo) → slate-950 (base).
    glBegin(GL_QUADS);
    glColor3f(0.059f, 0.090f, 0.165f); glVertex2f(0, 0); glVertex2f(windowWidth, 0);              // slate-900 topo
    glColor3f(0.024f, 0.031f, 0.059f); glVertex2f(windowWidth, windowHeight); glVertex2f(0, windowHeight); // slate-950 base
    glEnd();

    // Painel central: tudo é posicionado a partir de panelX (calculado pra
    // ficar sempre centralizado na janela, mesmo se ela for redimensionada).
    int panelWidth = menuAutoWallsW;
    int panelPaddingX = 36;
    int cardWidth = panelWidth + panelPaddingX * 2;
    int panelX = (windowWidth - panelWidth) / 2;

    // Altura total do conteúdo, calculada de antemão só para poder
    // centralizar o bloco inteiro verticalmente (e desenhar o card do
    // tamanho certo por baixo).
    int contentHeight = 34 /*título*/ + 28 /*subtítulo*/ + 20 /*espaço*/ + menuMinusH /*seletor*/
                       + 30 /*rótulo algoritmos*/ + 20 /*espaço*/ + menuAlgoH
                       + 22 + menuAutoWallsH + 22 + menuConfirmH;
    int cardPaddingY = 34;
    int cardHeight = contentHeight + cardPaddingY * 2;
    int cardY = std::max(20, (windowHeight - cardHeight) / 2);
    int cardX = panelX - panelPaddingX;

    // Sombra do card (p/ sensação de elevação), corpo em slate-800, e
    // listra de destaque indigo-500 no topo.
    drawRoundedRect(cardX + 4, cardY + 8, cardWidth, cardHeight, 18.0f, 0.0f, 0.0f, 0.0f, 0.40f);
    drawRoundedRect(cardX, cardY, cardWidth, cardHeight, 18.0f, 0.118f, 0.161f, 0.231f, 1.0f); // slate-800
    drawRoundedRect(cardX, cardY, cardWidth, 4, 18.0f, 0.388f, 0.396f, 0.945f, 0.95f);          // indigo-500 topo
    // Realce sutil na metade de cima do card, pra não ficar totalmente chapado.
    drawRoundedRect(cardX + 2, cardY + 2, cardWidth - 4, cardHeight / 2.5f, 16.0f, 1.0f, 1.0f, 1.0f, 0.025f);

    int y = cardY + cardPaddingY;

    glColor3f(1.0f, 1.0f, 1.0f);
    {
        const char* title = "Pathfinder3D";
        int tw = textWidth(GLUT_BITMAP_TIMES_ROMAN_24, title);
        glRasterPos2f(panelX + (panelWidth - tw) / 2.0f, y);
        for (const char* c = title; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
    y += 34;

    glColor3f(0.580f, 0.639f, 0.722f); // slate-400 — subtítulo secundário
    {
        const char* subtitle = "Configurar novo grid";
        int tw = textWidth(GLUT_BITMAP_HELVETICA_18, subtitle);
        glRasterPos2f(panelX + (panelWidth - tw) / 2.0f, y);
        for (const char* c = subtitle; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    y += 28 + 20;

    // Seletor de tamanho: uma "trilha" em pílula com "-" e "+" encaixados
    // nas pontas e o número no meio, visualmente uma peça só, em vez de
    // três retângulos soltos.
    int gridSize = controller.getPendingGridSize();
    char sizeText[32];
    snprintf(sizeText, sizeof(sizeText), "%d x %d", gridSize, gridSize);
    int sizeTextW = textWidth(GLUT_BITMAP_HELVETICA_18, sizeText);
    int sizeLabelW = 140;
    int rowW = menuMinusW + sizeLabelW + menuPlusW;
    int rowX = panelX + (panelWidth - rowW) / 2;

    drawRoundedRect(rowX, y, rowW, menuMinusH, menuMinusH / 2.0f, 0.200f, 0.255f, 0.333f, 1.0f); // slate-700

    menuMinusX = rowX;
    menuMinusY = y;
    drawButton(menuMinusX, menuMinusY, menuMinusW, menuMinusH, "-", false, GLUT_BITMAP_HELVETICA_18);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(rowX + menuMinusW + (sizeLabelW - sizeTextW) / 2.0f, y + menuMinusH / 2.0f + 6);
    for (const char* c = sizeText; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

    menuPlusX = rowX + menuMinusW + sizeLabelW;
    menuPlusY = y;
    drawButton(menuPlusX, menuPlusY, menuPlusW, menuPlusH, "+", false, GLUT_BITMAP_HELVETICA_18);

    y += menuMinusH + 30;

    glColor3f(0.580f, 0.639f, 0.722f); // slate-400
    {
        const char* algoLabel = "Algoritmos para comparar (tecla C na simulacao)";
        int tw = textWidth(GLUT_BITMAP_HELVETICA_12, algoLabel);
        glRasterPos2f(panelX + (panelWidth - tw) / 2.0f, y);
        for (const char* c = algoLabel; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    y += 20;

    // Chips de algoritmo: uma linha só, cada chip do tamanho do próprio
    // texto + preenchimento (estilo "tag selector"), centralizados como um
    // grupo. Bem mais compacto que 4 barras inteiras empilhadas.
    menuAlgoY = y;
    {
        const int chipPadding = 26;
        const int chipGap = 12;
        int totalChipsW = 0;
        int chipW[kAlgorithmCount];

        for (int i = 0; i < kAlgorithmCount; i++) {
            const char* name = algorithmName(kAllAlgorithmTypes[i]);
            chipW[i] = textWidth(GLUT_BITMAP_HELVETICA_18, name) + chipPadding * 2;
            totalChipsW += chipW[i];
        }
        totalChipsW += chipGap * (kAlgorithmCount - 1);

        int chipX = panelX + (panelWidth - totalChipsW) / 2;
        for (int i = 0; i < kAlgorithmCount; i++) {
            AlgorithmType type = kAllAlgorithmTypes[i];
            bool selected = controller.isAlgorithmSelected(type);

            menuAlgoChipX[i] = chipX;
            menuAlgoChipW[i] = chipW[i];
            drawButton(chipX, menuAlgoY, chipW[i], menuAlgoH, algorithmName(type), selected, GLUT_BITMAP_HELVETICA_18);

            chipX += chipW[i] + chipGap;
        }
    }
    y = menuAlgoY + menuAlgoH + 22;

    // Linha do toggle de paredes automáticas: rótulo à esquerda, switch à
    // direita, dentro de uma faixa sutilmente destacada do card.
    menuAutoWallsX = panelX;
    menuAutoWallsY = y;
    {
        bool autoWalls = controller.isAutoWallsEnabled();
        drawRoundedRect(menuAutoWallsX, menuAutoWallsY, menuAutoWallsW, menuAutoWallsH, 10.0f, 0.200f, 0.255f, 0.333f, 1.0f); // slate-700

        glColor3f(1.0f, 1.0f, 1.0f);
        const char* label = "Gerar paredes automaticamente";
        glRasterPos2f(menuAutoWallsX + 16, menuAutoWallsY + menuAutoWallsH / 2.0f + 5);
        for (const char* c = label; *c != '\0'; c++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

        float switchW = 46, switchH = 24;
        float switchX = menuAutoWallsX + menuAutoWallsW - switchW - 16;
        float switchY = menuAutoWallsY + (menuAutoWallsH - switchH) / 2.0f;
        drawToggleSwitch(switchX, switchY, switchW, switchH, autoWalls);
    }
    y += menuAutoWallsH + 22;

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

    // Fundo com gradiente vertical slate-900 → slate-950, igual ao menu,
    // para manter consistência visual entre telas.
    glBegin(GL_QUADS);
    glColor3f(0.059f, 0.090f, 0.165f); glVertex2f(0, 0); glVertex2f(windowWidth, 0);
    glColor3f(0.024f, 0.031f, 0.059f); glVertex2f(windowWidth, windowHeight); glVertex2f(0, windowHeight);
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

    // Card de fundo, mesmo tratamento do menu: sombra profunda, corpo
    // slate-800, listra de destaque indigo-500 no topo, glassy top sutil.
    int cardPaddingX = 30, cardPaddingY = 26;
    int cardWidth = std::max(tableWidth, buttonsRowWidth) + cardPaddingX * 2;
    int cardHeight = contentHeight + cardPaddingY * 2;
    int cardX = (windowWidth - cardWidth) / 2;
    int cardY = y - cardPaddingY;
    drawRoundedRect(cardX + 4, cardY + 8, cardWidth, cardHeight, 18.0f, 0.0f, 0.0f, 0.0f, 0.40f);
    drawRoundedRect(cardX, cardY, cardWidth, cardHeight, 18.0f, 0.118f, 0.161f, 0.231f, 1.0f); // slate-800
    drawRoundedRect(cardX, cardY, cardWidth, 4, 18.0f, 0.388f, 0.396f, 0.945f, 0.95f);          // indigo-500 topo
    drawRoundedRect(cardX + 2, cardY + 2, cardWidth - 4, cardHeight / 2.5f, 16.0f, 1.0f, 1.0f, 1.0f, 0.025f);

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

    // Fundo do cabeçalho em slate-700, neutro escuro que separa o header
    // das linhas de dados sem brigar com a cor indigo do card.
    glColor3f(0.200f, 0.255f, 0.333f); // slate-700
    glBegin(GL_QUADS);
    glVertex2f(tableX, y);
    glVertex2f(tableX + tableWidth, y);
    glVertex2f(tableX + tableWidth, y + headerH);
    glVertex2f(tableX, y + headerH);
    glEnd();

    glColor3f(0.886f, 0.910f, 0.941f); // slate-200 — texto do header, claro o bastante p/ contraste no slate-700
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

        // Linhas alternadas em slate-800 (par) e um tom mais escuro (ímpar).
        if (r % 2 == 0) glColor3f(0.118f, 0.161f, 0.231f);  // slate-800
        else             glColor3f(0.090f, 0.118f, 0.165f); // slate-800 mais escuro
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
    // Bordas da tabela em slate-600, separa colunas e linhas sem chamar
    // atenção demais (mais escuro que as linhas de dados, mas não preto).
    glColor3f(0.278f, 0.333f, 0.412f); // slate-600
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

    // Fundo equivalente ao piso cinza-claro das células Empty na cena 3D.
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

    // Cores originais da grid (não fazem parte do redesign de UI): mantidas
    // com matizes bem distintos entre si de propósito, para que dê pra
    // reconhecer cada categoria de longe/em miniatura, sem depender de
    // diferenças sutis de brilho dentro de uma mesma família de cor.
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
            // Cores originais da grid, mantidas com matizes distintos entre
            // si (ver nota em drawGridThumbnail).
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

    // Legenda em slate-200, claro o bastante para ler sobre o fundo
    // escuro da cena 3D, sem ser branco puro (mais suave aos olhos).
    glColor3f(0.886f, 0.910f, 0.941f); // slate-200
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
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawMenu();
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glutSwapBuffers();
        return;
    }

    if (state == AppState::Results) {
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawResults();
        glDisable(GL_BLEND);
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
            // Cores originais da grid, mantidas com matizes distintos entre
            // si (ver nota em drawGridThumbnail), o wireframe de contorno
            // dos cubos (em drawCube) continua valendo para todos eles.
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
    // que sobreponha um pixel já escrito na mesma profundidade (z=0).
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawMiniMap();
    glDisable(GL_BLEND);
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
        } else if (x >= menuAutoWallsX && x <= menuAutoWallsX + menuAutoWallsW &&
                   y >= menuAutoWallsY && y <= menuAutoWallsY + menuAutoWallsH) {
            controller.toggleAutoWalls();
        } else if (y >= menuAlgoY && y <= menuAlgoY + menuAlgoH) {
            for (int i = 0; i < kAlgorithmCount; i++) {
                if (x >= menuAlgoChipX[i] && x <= menuAlgoChipX[i] + menuAlgoChipW[i]) {
                    controller.toggleAlgorithmSelected(kAllAlgorithmTypes[i]);
                    break;
                }
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