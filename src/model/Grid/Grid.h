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

    // Realoca a matriz interna para o novo tamanho, limpando tudo para
    // CellType::Empty. Mantém a mesma instância de Grid viva (não recria o
    // objeto), então qualquer Grid& que outra classe já guarde (o Controller,
    // por exemplo) continua válida, só o conteúdo muda.
    void resize(int w, int h);

    CellType & at(int i, int y);
};