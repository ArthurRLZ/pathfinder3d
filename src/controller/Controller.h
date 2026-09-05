#pragma once

#include "model/Grid.h"
#include "model/Cell.h"
#include "model/AlgorithmType.h"
#include "model/ISearchAlgorithm.h"
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

    static Controller* instance;
    static void timerCallback(int value);
    void tick();

    void startAlgorithm();
    void resetGrid();
};