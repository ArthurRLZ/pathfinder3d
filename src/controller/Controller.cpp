#include "controller/Controller.h"
#include "../model/Algorithms/AlgorithmFactory/AlgorithmFactory.h"
#include <iostream>
#include <GL/glut.h>

Controller* Controller::instance = nullptr;

Controller::Controller(Grid& g)
    : grid(g), startPos(-1, -1), goalPos(-1, -1), mode(EditMode::None),
      currentAlgorithm(AlgorithmType::BFS), camera(nullptr), runState(RunState::Idle) {
    instance = this;
}

void Controller::setCamera(Camera* cam) {
    camera = cam;
}

void Controller::onKey(unsigned char key) {
    if (appState == AppState::Menu) {
        if (key == 13) confirmMenu(); // ENTER confirma o menu
        return;
    }
    if (appState == AppState::Results) {
        backToSimulation(); // qualquer tecla volta pra simulação
        return;
    }

    switch (key) {
        case '1':
            currentAlgorithm = AlgorithmType::BFS;
            std::cout << "Algoritmo selecionado: BFS\n";
            break;
        case '2':
            currentAlgorithm = AlgorithmType::ASTAR;
            std::cout << "Algoritmo selecionado: A-Star\n";
            break;
        case '3':
            currentAlgorithm = AlgorithmType::DFS;
            std::cout << "Algoritmo selecionado: DFS\n";
            break;
        case '4':
            currentAlgorithm = AlgorithmType::DIJKSTRA;
            std::cout << "Algoritmo selecionado: Dijkstra\n";
            break;
        case 's': case 'S':
            mode = EditMode::Start;
            std::cout << "Modo START\n";
            break;
        case 'g': case 'G':
            mode = EditMode::Goal;
            std::cout << "Modo GOAL\n";
            break;
        case 'w': case 'W':
            mode = EditMode::Wall;
            std::cout << "Modo WALL\n";
            break;
        case 'e': case 'E':
            mode = EditMode::Erase;
            std::cout << "Modo ERASE\n";
            break;
        case 'a': case 'A':
            if (camera) camera->orbitHorizontal(-kOrbitStepDeg);
            break;
        case 'd': case 'D':
            if (camera) camera->orbitHorizontal(kOrbitStepDeg);
            break;
        case 13: // ENTER
            startAlgorithm();
            break;
        case 'r': case 'R':
            resetGrid();
            break;
        case 'c': case 'C':
            runComparison();
            break;
    }
}

void Controller::onSpecialKey(int key) {
    if (!camera) return;
    switch (key) {
        case GLUT_KEY_LEFT:  camera->orbitHorizontal(-kOrbitStepDeg); break;
        case GLUT_KEY_RIGHT: camera->orbitHorizontal(kOrbitStepDeg); break;
        case GLUT_KEY_UP:    camera->orbitVertical(kOrbitStepDeg); break;
        case GLUT_KEY_DOWN:  camera->orbitVertical(-kOrbitStepDeg); break;
    }
}

void Controller::toggleTopView() {
    if (!camera) return;
    camera->toggleTopView();
    std::cout << (camera->isTopView() ? "Vista 2D (top-down)\n" : "Vista 3D (orbital)\n");
    glutTimerFunc(kStepIntervalMs, cameraTimerCallback, 0);
}

void Controller::cameraTimerCallback(int) {
    if (instance) instance->cameraTick();
}

void Controller::cameraTick() {
    if (!camera || !camera->isAnimating()) return;

    camera->animationStep(kCameraTransitionStepDeg);
    glutPostRedisplay();

    if (camera->isAnimating()) {
        glutTimerFunc(kStepIntervalMs, cameraTimerCallback, 0);
    }
}

void Controller::onMouse(int x, int y) {
    if (appState != AppState::Simulation) return;
    if (!grid.isInside(x, y)) return;
    if (runState == RunState::Running) return; // não editar a grid durante uma busca em andamento
    switch (mode) {
        case EditMode::Wall:
            if (grid.get(x, y) == CellType::Wall) grid.set(x, y, CellType::Empty);
            else if (grid.get(x, y) == CellType::Empty) grid.set(x, y, CellType::Wall);
            break;
        case EditMode::Erase:
            if (grid.get(x, y) != CellType::Start && grid.get(x, y) != CellType::Goal)
                grid.set(x, y, CellType::Empty);
            break;
        case EditMode::Start: setStart(x, y); break;
        case EditMode::Goal: setGoal(x, y); break;
        default: break;
    }
}

void Controller::setStart(int x, int y) {
    if (grid.get(x, y) == CellType::Wall) return;
    if (startPos.x >= 0) grid.set(startPos.x, startPos.y, CellType::Empty);
    startPos = {x, y};
    grid.set(x, y, CellType::Start);
}

void Controller::setGoal(int x, int y) {
    if (grid.get(x, y) == CellType::Wall) return;
    if (goalPos.x >= 0) grid.set(goalPos.x, goalPos.y, CellType::Empty);
    goalPos = {x, y};
    grid.set(x, y, CellType::Goal);
}

void Controller::startAlgorithm() {
    if (startPos.x < 0 || goalPos.x < 0) {
        std::cout << "Defina START e GOAL primeiro!\n";
        return;
    }

    if (runState == RunState::Running) {
        std::cout << "Já existe uma busca em andamento.\n";
        return;
    }

    for (int y = 0; y < grid.getHeight(); y++) {
        for (int x = 0; x < grid.getWidth(); x++) {
            if (grid.get(x, y) == CellType::Visited || grid.get(x, y) == CellType::Path) {
                grid.set(x, y, CellType::Empty);
            }
        }
    }

    activeAlgorithm = createAlgorithm(currentAlgorithm);
    activeAlgorithm->start(grid, startPos, goalPos);
    runState = RunState::Running;

    std::cout << "Buscando...\n";
    glutTimerFunc(kStepIntervalMs, timerCallback, 0);
}

void Controller::timerCallback(int) {
    if (instance) instance->tick();
}

void Controller::tick() {
    if (runState != RunState::Running || !activeAlgorithm) return;

    bool done = activeAlgorithm->step();
    glutPostRedisplay();

    if (!done) {
        glutTimerFunc(kStepIntervalMs, timerCallback, 0);
        return;
    }

    runState = RunState::Finished;

    if (!activeAlgorithm->found()) {
        std::cout << "Sem caminho.\n";
        return;
    }

    for (const auto& c : activeAlgorithm->getPath()) {
        CellType current = grid.get(c.x, c.y);
        if (current == CellType::Empty || current == CellType::Visited) {
            grid.set(c.x, c.y, CellType::Path);
        }
    }

    const SearchStats& stats = activeAlgorithm->getStats();
    std::cout << "Caminho encontrado! (" << activeAlgorithm->getPath().size()
              << " passos, " << stats.visitedCount << " celulas visitadas, "
              << stats.elapsedMs << " ms de computacao)\n";
}

void Controller::resetGrid() {
    activeAlgorithm.reset();
    runState = RunState::Idle;

    for (int y = 0; y < grid.getHeight(); y++)
        for (int x = 0; x < grid.getWidth(); x++)
            grid.set(x, y, CellType::Empty);
    startPos = {-1, -1};
    goalPos = {-1, -1};
    std::cout << "Grid resetado\n";
}

void Controller::increaseGridSize() {
    if (pendingGridSize < kMaxGridSize) pendingGridSize++;
}

void Controller::decreaseGridSize() {
    if (pendingGridSize > kMinGridSize) pendingGridSize--;
}

void Controller::toggleAlgorithmSelected(AlgorithmType t) {
    algorithmSelected[static_cast<int>(t)] = !algorithmSelected[static_cast<int>(t)];
}

void Controller::confirmMenu() {
    grid.resize(pendingGridSize, pendingGridSize);
    resetGrid();

    if (autoGenerateWalls) {
        WallGenerationResult result = gerarParedes(grid);
        startPos = result.start;
        goalPos = result.goal;
        if (result.success) {
            std::cout << "Paredes geradas automaticamente.\n";
        } else {
            std::cout << "Nao foi possivel gerar um labirinto conectado apos varias tentativas; grid ficou vazia.\n";
        }
    }

    if (camera) {
        // O centro geométrico e o raio "confortável" mudam junto com o
        // tamanho da grid, sem isso, a câmera continuaria orbitando o
        // centro do tamanho antigo.
        Vec3 newCenter(pendingGridSize / 2.0f, 0.0f, pendingGridSize / 2.0f);
        float newRadius = pendingGridSize * 1.3f + 5.0f;
        camera->setOrbit(newCenter, newRadius);
    }

    appState = AppState::Simulation;
}

void Controller::runComparison() {
    if (startPos.x < 0 || goalPos.x < 0) {
        std::cout << "Defina START e GOAL antes de comparar!\n";
        return;
    }
    if (runState == RunState::Running) {
        std::cout << "Aguarde a busca atual terminar antes de comparar.\n";
        return;
    }

    comparisonResults.clear();

    for (AlgorithmType type : kAllAlgorithmTypes) {
        if (!isAlgorithmSelected(type)) continue;

        // Cópia independente da grid atual: mesmas paredes/start/goal, mas
        // sem afetar a grid real nem a exibida em tela.
        Grid clone = grid;

        // Limpa Visited/Path que tenha sobrado de uma execução interativa
        // anterior (Enter) na grid real. Sem isso, BFS/DFS (que só andam
        // por cima de CellType::Empty ou CellType::Goal) tratariam essas
        // células como bloqueio, diferente de A*/Dijkstra (que só recusam
        // CellType::Wall) — mesma limpeza que startAlgorithm() já faz.
        for (int y = 0; y < clone.getHeight(); y++) {
            for (int x = 0; x < clone.getWidth(); x++) {
                CellType t = clone.get(x, y);
                if (t == CellType::Visited || t == CellType::Path) {
                    clone.set(x, y, CellType::Empty);
                }
            }
        }

        auto algo = createAlgorithm(type);
        algo->start(clone, startPos, goalPos);
        while (!algo->step()) {} // sem timer: roda tudo de uma vez, só para medir

        // Pinta o caminho encontrado por cima do Visited, igual tick() faz na
        // busca interativa, assim o "retrato" guardado abaixo fica idêntico
        // ao que apareceria no minimapa se essa fosse a busca ativa.
        if (algo->found()) {
            for (const auto& c : algo->getPath()) {
                CellType t = clone.get(c.x, c.y);
                if (t == CellType::Empty || t == CellType::Visited) {
                    clone.set(c.x, c.y, CellType::Path);
                }
            }
        }

        ComparisonRow row{
            type,
            algo->found(),
            static_cast<int>(algo->getPath().size()),
            algo->getStats().visitedCount,
            algo->getStats().elapsedMs,
            clone
        };
        comparisonResults.push_back(row);
    }

    appState = AppState::Results;
}