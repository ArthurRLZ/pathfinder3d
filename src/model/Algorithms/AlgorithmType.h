#pragma once

// Algoritmos de busca disponíveis para rodar sobre a grid.
enum class AlgorithmType { BFS, ASTAR, DFS, DIJKSTRA };

// Lista com todos os algoritmos, na ordem usada nas teclas de atalho (1-4),
// no menu inicial e no modo de comparação. Centralizada aqui para não
// duplicar esse array no Controller e no Renderer.
inline constexpr AlgorithmType kAllAlgorithmTypes[] = {
    AlgorithmType::BFS, AlgorithmType::ASTAR, AlgorithmType::DFS, AlgorithmType::DIJKSTRA
};
inline constexpr int kAlgorithmCount = 4;

inline const char* algorithmName(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::BFS:      return "BFS";
        case AlgorithmType::ASTAR:    return "A*";
        case AlgorithmType::DFS:      return "DFS";
        case AlgorithmType::DIJKSTRA: return "Dijkstra";
    }
    return "?";
}