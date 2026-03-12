#include "model/AStar.h"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>

struct AStarNode {
    Cell cell;
    int gCost;
    int hCost;
    int fCost() const { return gCost + hCost; }

    bool operator>(const AStarNode& other) const {
        return fCost() > other.fCost();
    }
};

struct CellHash {
    std::size_t operator()(const Cell& c) const {
        return std::hash<int>()(c.x) ^ (std::hash<int>()(c.y) << 1);
    }
};

int heuristic(Cell a, Cell b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

bool AStar::run(Grid& grid, Cell start, Cell goal, std::vector<Cell>& path) {
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openSet;
    std::unordered_map<Cell, Cell, CellHash> cameFrom;
    std::unordered_map<Cell, int, CellHash> gScore;

    openSet.push({start, 0, heuristic(start, goal)});
    gScore[start] = 0;

    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};

    while (!openSet.empty()) {
        Cell current = openSet.top().cell;
        openSet.pop();

        if (current == goal) {
            Cell curr = goal;
            while (!(curr == start)) {
                path.push_back(curr);
                curr = cameFrom[curr];
            }
            std::reverse(path.begin(), path.end());
            return true;
        }

        if (current != start && current != goal) {
            grid.set(current.x, current.y, CellType::Visited);
        }

        for (int i = 0; i < 4; i++) {
            Cell neighbor(current.x + dx[i], current.y + dy[i]);

            if (!grid.isInside(neighbor.x, neighbor.y) || grid.get(neighbor.x, neighbor.y) == CellType::Wall) {
                continue;
            }

            int tentative_gScore = gScore[current] + 1;

            if (gScore.find(neighbor) == gScore.end() || tentative_gScore < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentative_gScore;
                int h = heuristic(neighbor, goal);
                openSet.push({neighbor, tentative_gScore, h});
            }
        }
    }
    return false;
}