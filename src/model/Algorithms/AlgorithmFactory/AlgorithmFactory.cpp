#include "AlgorithmFactory.h"
#include "model/Algorithms/AStar/AStar.h"
#include "model/Algorithms/BFS/BFS.h"
#include "model/Algorithms/DFS/DFS.h"
#include "model/Algorithms/Dijkstra/Dijkstra.h"

std::unique_ptr<ISearchAlgorithm> createAlgorithm(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::BFS:      return std::make_unique<BFS>();
        case AlgorithmType::ASTAR:    return std::make_unique<AStar>();
        case AlgorithmType::DFS:      return std::make_unique<DFS>();
        case AlgorithmType::DIJKSTRA: return std::make_unique<Dijkstra>();
    }
    return nullptr;
}
