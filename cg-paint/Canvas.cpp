#include <vector>
#include <math.h>
#include "Canvas.h"

void Canvas::resize(int newW, int newH, int offX, int offY) {
    std::vector<Color> newCells(newW * newH, Color{ 255, 255, 255, 255 });

    for (int y = 0; y < newH; y++) {
        for (int x = 0; x < newW; x++) {
            int ox = x + offX, oy = y + offY;
            if (inside(ox, oy))
                newCells[y * newW + x] = cells[oy * w + ox];
        }
    }

    w = newW;
    h = newH;
    cells = std::move(newCells);
}



void Canvas::flipHorizontal() {
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w / 2; x++)
            std::swap(cells[y * w + x], cells[y * w + (w - 1 - x)]);
}

void Canvas::flipVertical() {
    for (int y = 0; y < h / 2; y++)
        for (int x = 0; x < w; x++)
            std::swap(cells[y * w + x], cells[(h - 1 - y) * w + x]);
}

void Canvas::rotateClockwise() {
    std::vector<Color> out(w * h);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            out[x * h + (h - 1 - y)] = cells[y * w + x];
    cells = std::move(out);
    std::swap(w, h);
}

void Canvas::rotateCounterClockwise() {
    std::vector<Color> out(w * h);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            out[(w - 1 - x) * h + y] = cells[y * w + x];
    cells = std::move(out);
    std::swap(w, h);
}