#pragma once
#include <vector>
#include "Canvas.h"

void Canvas::resize(int newW, int newH) {
    std::vector<Color> newCells(
        newW * newH,
        Color{ 0.0f, 0.0f, 0.0f, 1.0f }
    );

    int copyW = std::min(w, newW);
    int copyH = std::min(h, newH);

    for (int y = 0; y < copyH; y++) {
        for (int x = 0; x < copyW; x++) {
            newCells[y * newW + x] = cells[y * w + x];
        }
    }

    w = newW;
    h = newH;
    cells = std::move(newCells);
}