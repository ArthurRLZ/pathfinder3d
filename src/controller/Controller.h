#pragma once

#include "../model/Grid/Grid.h"
#include "../model/Cell/Cell.h"
#include "../model/Algorithms/AlgorithmType.h"
#include "../model/Algorithms/ISearchAlgorithm.h"
#include "view/Camera.h"
#include <memory>

enum class EditMode { None, Start, Goal, Wall, Erase };

// Estado da execução do algoritmo de busca ativo.
enum class RunState { Idle, Running, Finished };

class Controller {
public:
    Controller(Grid& grid);
    void setCamera(Camera* cam);
    void setStart(int x, int y);
    void setGoal(int x, int y);
    void onKey(unsigned char key);
    void onSpecialKey(int key);
    void onMouse(int gridX, int gridY);
    void setStart(const Cell& c);
    void setGoal(const Cell& c);
    void addWall(const Cell& c);
    void clearCell(const Cell& c);

    bool isRunning() const { return runState == RunState::Running; }
    bool isTopView() const { return camera && camera->isTopView(); }
    void toggleTopView();

private:
    Grid& grid;
    Cell startPos;
    Cell goalPos;
    EditMode mode;
    AlgorithmType currentAlgorithm;
    Camera* camera;

    std::unique_ptr<ISearchAlgorithm> activeAlgorithm;
    RunState runState;

    // Intervalo entre passos da animação da busca, em milissegundos.
    // Quanto menor, mais rápida a animação.
    static const int kStepIntervalMs = 15;

    // Quantos graus a câmera orbita por tecla pressionada.
    static constexpr float kOrbitStepDeg = 5.0f;

    static Controller* instance;
    static void timerCallback(int value);
    void tick();

    void startAlgorithm();
    void resetGrid();
};