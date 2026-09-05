#include "model/AlgorithmFactory.h"
#include "model/BFS.h"
#include "model/AStar.h"
#include "model/DFS.h"
#include "model/Dijkstra.h"

std::unique_ptr<ISearchAlgorithm> createAlgorithm(AlgorithmType type) {
    switch (type) {
        case AlgorithmType::BFS:      return std::make_unique<BFS>();
        case AlgorithmType::ASTAR:    return std::make_unique<AStar>();
        case AlgorithmType::DFS:      return std::make_unique<DFS>();
        case AlgorithmType::DIJKSTRA: return std::make_unique<Dijkstra>();
    }
    return nullptr;
}