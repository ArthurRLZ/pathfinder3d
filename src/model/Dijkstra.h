#pragma once
#include "model/ISearchAlgorithm.h"
#include <queue>
#include <unordered_map>
#include <vector>
#include <chrono>

// Dijkstra é matematicamente A* com heurística sempre 0 (busca por custo
// uniforme): sem "palpite" de distância até o destino, expande estritamente
// por ordem de custo acumulado (gCost). A estrutura é praticamente idêntica
// à do AStar; a única diferença de fato é heuristic() sempre retornar 0.
class Dijkstra : public ISearchAlgorithm {
public:
    void start(Grid& grid, Cell start, Cell goal) override;
    bool step() override;

    bool found() const override { return foundFlag; }
    const std::vector<Cell>& getPath() const override { return path; }
    const SearchStats& getStats() const override { return stats; }

private:
    struct Node {
        Cell cell;
        int gCost;
        int fCost() const { return gCost; } // hCost é sempre 0
        bool operator>(const Node& other) const { return fCost() > other.fCost(); }
    };

    struct CellHash {
        std::size_t operator()(const Cell& c) const {
            return std::hash<int>()(c.x) ^ (std::hash<int>()(c.y) << 1);
        }
    };

    Grid* grid = nullptr;
    Cell startCell;
    Cell goalCell;

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<Cell, Cell, CellHash> cameFrom;
    std::unordered_map<Cell, int, CellHash> gScore;

    std::vector<Cell> path;
    SearchStats stats;
    bool foundFlag = false;
    bool finished = false;
    std::chrono::steady_clock::time_point tStart;

    void buildPath();
    void finish();
};