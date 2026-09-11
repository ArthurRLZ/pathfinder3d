#include "WallGenerator.h"
#include "model/Algorithms/BFS/BFS.h"
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <vector>

namespace {

void randomFill(Grid& grid, float densidade) {
    for (int y = 0; y < grid.getHeight(); y++) {
        for (int x = 0; x < grid.getWidth(); x++) {
            float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
            grid.set(x, y, r < densidade ? CellType::Wall : CellType::Empty);
        }
    }
}

int countWallNeighbors(Grid& grid, int x, int y) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            // Fora da grid conta como parede: assim as bordas "encolhem"
            // naturalmente durante a suavização, em vez de vazar caverna
            // para fora dos limites.
            if (!grid.isInside(nx, ny) || grid.get(nx, ny) == CellType::Wall) {
                count++;
            }
        }
    }
    return count;
}

// Uma passada do autômato celular: cada célula vira parede se tiver 5 ou
// mais vizinhas-parede (vizinhança 8-direcional), senão vira vazia. Isso é
// o que transforma o ruído aleatório inicial em formas orgânicas de caverna.
void smoothStep(Grid& grid) {
    int w = grid.getWidth();
    int h = grid.getHeight();
    std::vector<std::vector<CellType>> next(h, std::vector<CellType>(w));

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            next[y][x] = (countWallNeighbors(grid, x, y) >= 5) ? CellType::Wall : CellType::Empty;
        }
    }

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            grid.set(x, y, next[y][x]);
}

// A suavização por autômato celular sozinha tende a empurrar a parede pra
// perto das bordas (a regra "fora da grid conta como parede" favorece a
// borda a cada passada), deixando o interior quase vazio, mais notável em
// grids pequenas/médias, onde "perto da borda" é boa parte da área total.
//
// Por isso espalhamos blocos retangulares de parede pelo interior depois da
// suavização: mantém a textura orgânica das bordas e garante obstáculos
// reais no meio do caminho, não só uma moldura oca.
void sprinkleInteriorBlobs(Grid& grid) {
    int w = grid.getWidth();
    int h = grid.getHeight();

    // Grids muito pequenas não têm espaço pra blocos de 2x2+ com margem.
    if (w < 8 || h < 8) return;

    int blobCount = std::max(4, (w * h) / 40);

    for (int b = 0; b < blobCount; b++) {
        int blobW = 2 + std::rand() % 3; // 2..4
        int blobH = 2 + std::rand() % 3;
        blobW = std::min(blobW, w - 2);
        blobH = std::min(blobH, h - 2);

        int blobX = 1 + std::rand() % std::max(1, w - blobW - 2);
        int blobY = 1 + std::rand() % std::max(1, h - blobH - 2);

        for (int y = blobY; y < blobY + blobH; y++) {
            for (int x = blobX; x < blobX + blobW; x++) {
                if (grid.isInside(x, y)) grid.set(x, y, CellType::Wall);
            }
        }
    }
}

// Acha a célula vazia mais próxima de (startX, startY), varrendo em anéis de
// raio crescente (distância de Chebyshev). Usado para "empurrar" Start/Goal
// para fora de uma parede, caso o canto escolhido tenha calhado de virar
// parede durante a geração.
Cell nearestEmptyCell(Grid& grid, int startX, int startY) {
    if (grid.isInside(startX, startY) && grid.get(startX, startY) == CellType::Empty) {
        return Cell(startX, startY);
    }

    int maxRadius = grid.getWidth() + grid.getHeight();
    for (int radius = 1; radius <= maxRadius; radius++) {
        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                if (std::max(std::abs(dx), std::abs(dy)) != radius) continue; // só a borda deste anel

                int nx = startX + dx;
                int ny = startY + dy;
                if (grid.isInside(nx, ny) && grid.get(nx, ny) == CellType::Empty) {
                    return Cell(nx, ny);
                }
            }
        }
    }

    return Cell(startX, startY); // não deveria acontecer se houver ao menos 1 célula vazia
}

// Roda o BFS já existente até o fim (sem timer/animação) só pra checar se
// existe caminho entre start e goal. BFS::step() reconhece o destino pelo
// CellType da célula (precisa ser CellType::Goal), não pelas coordenadas,
// por isso marcamos o destino temporariamente antes de rodar, e desfazemos
// a marca ao final.
bool isConnected(Grid& grid, Cell start, Cell goal) {
    CellType originalGoalType = grid.get(goal.x, goal.y);
    grid.set(goal.x, goal.y, CellType::Goal);

    BFS bfs;
    bfs.start(grid, start, goal);
    while (!bfs.step()) {}
    bool found = bfs.found();

    grid.set(goal.x, goal.y, originalGoalType);
    return found;
}

void clearVisitedMarks(Grid& grid) {
    for (int y = 0; y < grid.getHeight(); y++)
        for (int x = 0; x < grid.getWidth(); x++)
            if (grid.get(x, y) == CellType::Visited) grid.set(x, y, CellType::Empty);
}

} // namespace

WallGenerationResult gerarParedes(Grid& grid, float densidade, int maxAttempts) {
    int w = grid.getWidth();
    int h = grid.getHeight();
    const int kSmoothIterations = 5;

    for (int attempt = 0; attempt < maxAttempts; attempt++) {
        randomFill(grid, densidade);

        for (int i = 0; i < kSmoothIterations; i++) {
            smoothStep(grid);
        }

        sprinkleInteriorBlobs(grid);

        Cell start = nearestEmptyCell(grid, 0, 0);
        Cell goal = nearestEmptyCell(grid, w - 1, h - 1);

        bool connected = isConnected(grid, start, goal);
        clearVisitedMarks(grid);

        if (connected) {
            grid.set(start.x, start.y, CellType::Start);
            grid.set(goal.x, goal.y, CellType::Goal);
            return { true, start, goal };
        }
    }

    // Não conseguiu gerar uma configuração conectada a tempo: cai para uma
    // grid totalmente vazia (sempre conectada) em vez de deixar o usuário
    // preso numa grid sem solução.
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            grid.set(x, y, CellType::Empty);

    Cell start(0, 0);
    Cell goal(w - 1, h - 1);
    grid.set(start.x, start.y, CellType::Start);
    grid.set(goal.x, goal.y, CellType::Goal);
    return { false, start, goal };
}