#pragma once

#include "model/Grid/Grid.h"
#include "model/Cell/Cell.h"

// Resultado da geração automática de paredes. A função também escolhe
// Start/Goal, já que roda antes de o usuário editar a grid manualmente.
struct WallGenerationResult {
    bool success; // false só se nem em maxAttempts tentativas achou uma configuração conectada
    Cell start;
    Cell goal;
};

// Gera paredes em 'grid' usando autômato celular (cave generation): densidade
// inicial de parede via 'densidade', suavizado em várias passadas até formar
// cavernas orgânicas. Escolhe Start/Goal em cantos opostos (ou a célula vazia
// mais próxima, se o canto virar parede) e valida conectividade com BFS,
// tentando de novo até 'maxAttempts' vezes; se falhar sempre, cai para uma
// grid vazia (sempre conectada).
WallGenerationResult gerarParedes(Grid& grid, float densidade = 0.42f, int maxAttempts = 20);