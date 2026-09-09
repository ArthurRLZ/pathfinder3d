#pragma once

#include "../model/Grid/Grid.h"
#include "../model/Cell/Cell.h"
#include "../model/Algorithms/AlgorithmType.h"
#include "../model/Algorithms/ISearchAlgorithm.h"
#include "view/Camera.h"
#include <memory>
#include <vector>
#include <array>

enum class EditMode { None, Start, Goal, Wall, Erase };

// Estado da execução do algoritmo de busca ativo.
enum class RunState { Idle, Running, Finished };

// Em que tela o programa está. Menu = escolhendo tamanho/algoritmos antes de
// começar; Simulation = grid 3D normal; Results = tabela de comparação.
enum class AppState { Menu, Simulation, Results };

// Uma linha da tabela de comparação: o resultado de um algoritmo rodando
// sobre a mesma grid/obstáculos/start/goal que os outros.
struct ComparisonRow {
    AlgorithmType type;
    bool found;
    int pathLength;
    int visitedCount;
    double elapsedMs;
};

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

    // --- Menu inicial e comparação de algoritmos ---
    AppState getAppState() const { return appState; }
    int getPendingGridSize() const { return pendingGridSize; }
    bool isAlgorithmSelected(AlgorithmType t) const { return algorithmSelected[static_cast<int>(t)]; }
    const std::vector<ComparisonRow>& getComparisonResults() const { return comparisonResults; }

    void increaseGridSize();
    void decreaseGridSize();
    void toggleAlgorithmSelected(AlgorithmType t);
    void confirmMenu();
    void runComparison();
    void backToSimulation() { appState = AppState::Simulation; }

private:
    Grid& grid;
    Cell startPos;
    Cell goalPos;
    EditMode mode;
    AlgorithmType currentAlgorithm;
    Camera* camera;

    std::unique_ptr<ISearchAlgorithm> activeAlgorithm;
    RunState runState;

    AppState appState = AppState::Menu;
    int pendingGridSize = 20;
    std::array<bool, kAlgorithmCount> algorithmSelected{ {true, true, true, true} };
    std::vector<ComparisonRow> comparisonResults;

    static const int kMinGridSize = 2;
    static const int kMaxGridSize = 60;

    // Intervalo entre passos da animação da busca, em milissegundos.
    // Quanto menor, mais rápida a animação.
    static const int kStepIntervalMs = 15;

    // Quantos graus a câmera orbita por tecla pressionada.
    static constexpr float kOrbitStepDeg = 5.0f;

    // Quantos graus a câmera avança por frame durante a transição animada
    // entre a órbita livre e a vista de topo (menor = mais suave e lento).
    static constexpr float kCameraTransitionStepDeg = 2.0f;

    static Controller* instance;
    static void timerCallback(int value);
    void tick();

    static void cameraTimerCallback(int value);
    void cameraTick();

    void startAlgorithm();
    void resetGrid();
};