#include "BFS.h"
#include <queue>

bool BFS::run(Grid& grid, Cell start, Cell goal, std::vector<Cell>& outPath) {
    std::queue<Cell> q;
    q.push(start);

    int h = grid.getHeight();
    int w = grid.getWidth();

    std::vector<std::vector<Cell>> parent(
        h, std::vector<Cell>(w, {-1, -1})
    );

    parent[start.y][start.x] = start;

    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};

    if (start.x == goal.x && start.y == goal.y) {
        outPath.push_back(start);
        return true;
    }

    while (!q.empty()) {
        Cell cur = q.front();
        q.pop();

        for (int i = 0; i < 4; i++) {
            Cell next = {cur.x + dx[i], cur.y + dy[i]};

            if (!grid.isInside(next.x, next.y))
                continue;

            CellType value = grid.get(next.x, next.y);

            if (value != CellType::Empty && value != CellType::Goal)
                continue;

            if (value == CellType::Goal) {
                parent[next.y][next.x] = cur;
                
                Cell p = goal;

                while (!(p.x == start.x && p.y == start.y)) {
                    outPath.push_back(p);
                    p = parent[p.y][p.x];
                }
                outPath.push_back(start);
                return true;
            }

            if (value == CellType::Empty) {
                grid.set(next.x, next.y, CellType::Visited);
                parent[next.y][next.x] = cur;
                q.push(next);
            }
        }
    }

    return false;
}