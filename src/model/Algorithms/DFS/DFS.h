#pragma once
#include "../ISearchAlgorithm.h"
#include <stack>
#include <vector>
#include <chrono>

class DFS : public ISearchAlgorithm {
public:
    void start(Grid& grid, Cell start, Cell goal) override;
    bool step() override;

    bool found() const override { return foundFlag; }
    const std::vector<Cell>& getPath() const override { return path; }
    const SearchStats& getStats() const override { return stats; }

private:
    Grid* grid = nullptr;
    Cell startCell;
    Cell goalCell;

    // Pilha em vez de fila: é a única diferença estrutural em relação ao BFS.
    // Processar o topo da pilha (LIFO) faz a busca aprofundar em um ramo
    // antes de voltar para explorar os outros, que é a essência do DFS.
    std::stack<Cell> frontier;
    std::vector<std::vector<Cell>> parent;

    std::vector<Cell> path;
    SearchStats stats;
    bool foundFlag = false;
    bool finished = false;
    std::chrono::steady_clock::time_point tStart;

    void buildPath();
    void finish();
};