#pragma once
#include "model/ISearchAlgorithm.h"
#include <queue>
#include <vector>
#include <chrono>

class BFS : public ISearchAlgorithm {
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

    std::queue<Cell> frontier;
    std::vector<std::vector<Cell>> parent;

    std::vector<Cell> path;
    SearchStats stats;
    bool foundFlag = false;
    bool finished = false;
    std::chrono::steady_clock::time_point tStart;

    void buildPath();
    void finish();
};