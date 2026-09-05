#include "Dijkstra.h"
#include <algorithm>

void Dijkstra::start(Grid& g, Cell s, Cell goalIn) {
    grid = &g;
    startCell = s;
    goalCell = goalIn;

    path.clear();
    stats = SearchStats{};
    foundFlag = false;
    finished = false;

    while (!openSet.empty()) openSet.pop();
    cameFrom.clear();
    gScore.clear();

    openSet.push({startCell, 0});
    gScore[startCell] = 0;

    tStart = std::chrono::steady_clock::now();
}

bool Dijkstra::step() {
    if (finished) return true;

    if (openSet.empty()) {
        foundFlag = false;
        finish();
        return true;
    }

    Cell current = openSet.top().cell;
    openSet.pop();

    if (current == goalCell) {
        buildPath();
        foundFlag = true;
        finish();
        return true;
    }

    if (current != startCell && current != goalCell &&
        grid->get(current.x, current.y) != CellType::Visited) {
        grid->set(current.x, current.y, CellType::Visited);
        stats.visitedCount++;
    }

    static const int dx[4] = {0, 1, 0, -1};
    static const int dy[4] = {-1, 0, 1, 0};

    for (int i = 0; i < 4; i++) {
        Cell neighbor(current.x + dx[i], current.y + dy[i]);

        if (!grid->isInside(neighbor.x, neighbor.y) ||
            grid->get(neighbor.x, neighbor.y) == CellType::Wall) {
            continue;
        }

        int tentative_gScore = gScore[current] + 1;

        auto it = gScore.find(neighbor);
        if (it == gScore.end() || tentative_gScore < it->second) {
            cameFrom[neighbor] = current;
            gScore[neighbor] = tentative_gScore;
            openSet.push({neighbor, tentative_gScore});
        }
    }

    return false;
}

void Dijkstra::buildPath() {
    Cell curr = goalCell;
    while (!(curr == startCell)) {
        path.push_back(curr);
        curr = cameFrom[curr];
    }
    path.push_back(startCell);
    std::reverse(path.begin(), path.end());
}

void Dijkstra::finish() {
    finished = true;
    auto elapsed = std::chrono::steady_clock::now() - tStart;
    stats.elapsedMs = std::chrono::duration<double, std::milli>(elapsed).count();
}