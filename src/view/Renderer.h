#pragma once

#include "../model/Grid/Grid.h"
#include "../model/Cell/Cell.h"
#include "controller/Controller.h"
#include "view/Camera.h"

class Renderer {
public:
    Renderer(Grid& grid, Controller& controller);

    void run(int argc, char** argv);
    void setCamera(Camera* cam);

private:
    Grid& grid;
    Controller& controller;
    Camera* camera = nullptr;

    int windowWidth = 800;
    int windowHeight = 600;

    static Renderer* instance;

    static void displayCallback();
    static void reshapeCallback(int w, int h);
    static void keyboardCallback(unsigned char key, int x, int y);
    static void specialKeyboardCallback(int key, int x, int y);
    static void mouseCallback(int button, int state, int x, int y);

    void display();
    void reshape(int w, int h);
    void keyboard(unsigned char key);
    void specialKeyboard(int key);
    void mouse(int button, int state, int x, int y);

    void drawCube(int x, int z, float r, float g, float b);
    void drawMiniMap();

    // Tela de menu inicial (AppState::Menu): escolher tamanho do grid e
    // quais algoritmos entram na comparação. Layout centralizado na janela;
    // as posições dos controles são recalculadas a cada frame em drawMenu()
    // a partir de windowWidth/windowHeight, então mouse() sempre compara
    // contra a posição atual desenhada.
    void drawMenu();
    int menuMinusX = 0, menuMinusY = 0, menuMinusW = 44, menuMinusH = 40;
    int menuPlusX = 0, menuPlusY = 0, menuPlusW = 44, menuPlusH = 40;
    // Checkboxes de algoritmo: empilhados verticalmente a partir de
    // (menuAlgoX, menuAlgoY), um a cada menuAlgoSpacing pixels.
    int menuAlgoX = 0, menuAlgoY = 0, menuAlgoW = 360, menuAlgoH = 40, menuAlgoSpacing = 50;
    int menuAutoWallsX = 0, menuAutoWallsY = 0, menuAutoWallsW = 360, menuAutoWallsH = 40;
    int menuConfirmX = 0, menuConfirmY = 0, menuConfirmW = 360, menuConfirmH = 50;

    // Tela de resultados (AppState::Results): tabela comparativa, layout
    // centralizado.
    void drawResults();
    void drawGridThumbnail(const Grid& g, float x, float y, float size);
    int resultsBackX = 0, resultsBackY = 0, resultsBackW = 220, resultsBackH = 44;
    int resultsMenuX = 0, resultsMenuY = 0, resultsMenuW = 220, resultsMenuH = 44;

    // Botão "Voltar ao menu", visível na tela de simulação (desenhado dentro
    // de drawMiniMap(), junto do botão de vista 2D/3D).
    int simMenuButtonX = 0, simMenuButtonY = 0, simMenuButtonW = 180, simMenuButtonH = 30;

    // Desenha um retângulo clicável com rótulo centralizado, dentro de um
    // bloco ortho2D já aberto pelo chamador. Reaproveitado pelo menu, pelos
    // resultados e pelos botões da simulação. 'font' controla o tamanho do
    // texto (use GLUT_BITMAP_HELVETICA_18 para botões normais e
    // GLUT_BITMAP_HELVETICA_12 para botões pequenos/secundários).
    void drawButton(int x, int y, int w, int h, const char* label, bool highlighted, void* font);
    int textWidth(void* font, const char* text);

    int miniMapX = 20;
    int miniMapY = 20;
    int miniMapSize = 200;

    // Retângulo do botão de alternar vista 2D/3D, em coordenadas de tela.
    // Recalculado a cada frame em drawMiniMap(), para que mouse() sempre
    // compare o clique contra a posição atual desenhada (evita desalinhar
    // os dois se a posição mudar no futuro).
    int topViewButtonX = 0;
    int topViewButtonY = 0;
    int topViewButtonW = 180;
    int topViewButtonH = 30;
};