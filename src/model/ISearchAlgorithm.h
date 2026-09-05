#pragma once
#include "model/Grid.h"
#include "model/Cell.h"
#include <vector>

// Estatísticas coletadas durante uma execução de busca.
// elapsedMs mede apenas o tempo de computação real (soma do tempo gasto
// dentro de cada chamada a step()), não o tempo "de parede" da animação,
// que é dominado pelo intervalo artificial entre frames.
struct SearchStats {
    int visitedCount = 0;
    double elapsedMs = 0.0;
};

// Interface comum para algoritmos de busca em grid que podem ser executados
// de forma "stepada" (uma unidade de trabalho por chamada), permitindo que o
// Controller anime a exploração célula por célula em vez de rodar tudo de
// uma vez. Cada algoritmo concreto (BFS, AStar, futuramente DFS/Dijkstra)
// implementa essa interface.
class ISearchAlgorithm {
public:
    virtual ~ISearchAlgorithm() = default;

    // Prepara o algoritmo para buscar de 'start' até 'goal' em 'grid'.
    // Não executa nenhuma iteração ainda.
    virtual void start(Grid& grid, Cell start, Cell goal) = 0;

    // Processa uma unidade de trabalho (ex: expande um nó da fronteira).
    // Retorna true quando o algoritmo terminou, seja porque achou o
    // caminho, seja porque esgotou a busca sem sucesso.
    virtual bool step() = 0;

    // Só é válido chamar depois que step() retornou true.
    virtual bool found() const = 0;
    virtual const std::vector<Cell>& getPath() const = 0;
    virtual const SearchStats& getStats() const = 0;
};