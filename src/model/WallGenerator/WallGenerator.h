#pragma once

#include "model/Grid/Grid.h"
#include "model/Cell/Cell.h"

// Resultado da geração automática de paredes. Além de preencher a Grid com
// obstáculos, a função também escolhe onde ficam Start e Goal — no momento
// em que isso roda (logo após redimensionar a grid no menu inicial), o
// usuário ainda não editou nada manualmente, então não há Start/Goal prévios
// para reaproveitar.
struct WallGenerationResult {
    bool success; // false só se nem em maxAttempts tentativas achou uma configuração conectada
    Cell start;
    Cell goal;
};

// Gera paredes em 'grid' usando autômato celular (estilo "cave generation"):
// preenche aleatoriamente com probabilidade 'densidade' de virar parede,
// depois suaviza o ruído em várias passadas até formar cavernas orgânicas.
// Escolhe Start/Goal em cantos opostos (ou a célula vazia mais próxima
// deles, se o canto exato tiver virado parede) e valida a conectividade
// entre os dois com BFS. Se não houver caminho, tenta gerar de novo, até
// 'maxAttempts' vezes; se mesmo assim falhar, cai para uma grid vazia
// (sempre conectada) em vez de deixar o usuário preso sem solução.
WallGenerationResult gerarParedes(Grid& grid, float densidade = 0.42f, int maxAttempts = 20);