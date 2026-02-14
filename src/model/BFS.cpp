#include "BFS.h"
#include <queue>

bool BFS::run(Grid& grid, Cell start, Cell goal, std::vector<Cell>& outPath) {
    std::queue<Cell> q;
    q.push(start);

    int h = grid.getHeight();
    int w = grid.getWidth();

    // Matriz para reconstrução do caminho
    std::vector<std::vector<Cell>> parent(
        h, std::vector<Cell>(w, {-1, -1})
    );

    parent[start.y][start.x] = start;

    // Direções: direita, esquerda, baixo, cima
    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};

    while (!q.empty()) {
        Cell cur = q.front();
        q.pop();

        // Chegou ao objetivo
        if (cur.x == goal.x && cur.y == goal.y) {
            Cell p = goal;

            // Reconstrói o caminho
            while (!(p.x == start.x && p.y == start.y)) {
                outPath.push_back(p);
                p = parent[p.y][p.x];
            }
            outPath.push_back(start);
            return true;
        }

        // Explora vizinhos
        for (int i = 0; i < 4; i++) {
            Cell next = {cur.x + dx[i], cur.y + dy[i]};

            // Fora do grid
            if (!grid.isInside(next.x, next.y))
                continue;

            int value = grid.get(next.x, next.y);

            // Bloqueia parede, visitado, etc.
            // Permite chão (0) e goal (3)
            if (value != 0 && value != 3)
                continue;

            // Marca como visitado (exceto o goal)
            if (value == 0)
                grid.set(next.x, next.y, 4);

            parent[next.y][next.x] = cur;
            q.push(next);
        }
    }

    // Nenhum caminho encontrado
    return false;
}
