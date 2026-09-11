#pragma once
#include <vector>
#include "../Cell/CellType.h"

class Grid {
private:
    int width;
    int height;
    std::vector<std::vector<CellType>> cells;

public:
    Grid(int w, int h);

    CellType get(int x, int y) const;
    void set(int x, int y, CellType value);

    bool isInside(int x, int y) const;

    int getWidth() const;
    int getHeight() const;

    // Realoca a matriz interna para o novo tamanho (tudo vira Empty).
    // Mantém a mesma instância viva em vez de recriar o objeto, para que
    // referências existentes (Grid& no Controller, por exemplo) continuem válidas.
    void resize(int w, int h);

    CellType & at(int i, int y);
};