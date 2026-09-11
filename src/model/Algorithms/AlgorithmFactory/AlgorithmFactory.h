#pragma once
#include "../AlgorithmType.h"
#include "../ISearchAlgorithm.h"
#include <memory>

// Cria a instância concreta do algoritmo de busca correspondente ao tipo
// pedido. Centraliza esse mapeamento num único lugar, para que adicionar um
// algoritmo novo não exija tocar em if/else espalhados pelo Controller e
// pelo modo de comparação.
std::unique_ptr<ISearchAlgorithm> createAlgorithm(AlgorithmType type);