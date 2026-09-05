#include "BFS.h"
#include <algorithm>

void BFS::start(Grid& g, Cell s, Cell goalIn) {
    grid = &g;
    startCell = s;
    goalCell = goalIn;

    path.clear();
    stats = SearchStats{};
    foundFlag = false;
    finished = false;

    while (!frontier.empty()) frontier.pop();
    frontier.push(startCell);

    int h = grid->getHeight();
    int w = grid->getWidth();
    parent.assign(h, std::vector<Cell>(w, Cell(-1, -1)));
    parent[startCell.y][startCell.x] = startCell;

    tStart = std::chrono::steady_clock::now();

    // Caso trivial: start e goal são a mesma célula.
    if (startCell == goalCell) {
        path.push_back(startCell);
        foundFlag = true;
        finish();
    }
}

bool BFS::step() {
    if (finished) return true;

    if (frontier.empty()) {
        foundFlag = false;
        finish();
        return true;
    }

    Cell current = frontier.front();
    frontier.pop();

    static const int dx[4] = {1, -1, 0, 0};
    static const int dy[4] = {0, 0, 1, -1};

    for (int i = 0; i < 4; i++) {
        Cell next(current.x + dx[i], current.y + dy[i]);

        if (!grid->isInside(next.x, next.y))
            continue;

        CellType value = grid->get(next.x, next.y);

        if (value != CellType::Empty && value != CellType::Goal)
            continue;

        if (value == CellType::Goal) {
            parent[next.y][next.x] = current;
            buildPath();
            foundFlag = true;
            finish();
            return true;
        }

        grid->set(next.x, next.y, CellType::Visited);
        stats.visitedCount++;
        parent[next.y][next.x] = current;
        frontier.push(next);
    }

    return false;
}

void BFS::buildPath() {
    Cell p = goalCell;
    while (!(p == startCell)) {
        path.push_back(p);
        p = parent[p.y][p.x];
    }
    path.push_back(startCell);
    std::reverse(path.begin(), path.end());
}

void BFS::finish() {
    finished = true;
    auto elapsed = std::chrono::steady_clock::now() - tStart;
    stats.elapsedMs = std::chrono::duration<double, std::milli>(elapsed).count();
}