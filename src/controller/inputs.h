#pragma once

#include "model/Cell.h"
#include "model/Grid.h"

void handleKeyboardInput(unsigned char key, int x, int y, Grid& grid, Cell& startPos, Cell& goalPos);
