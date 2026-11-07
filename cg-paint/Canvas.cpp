#include <vector>
#include <math.h>
#include "Canvas.h"

void Canvas::resize(int newW, int newH) {
    std::vector<Color> newCells(
        newW * newH,
        Color{ 0, 0, 0, 255 }
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

View::View(const Canvas& canvas, Workspace ws, float marginPercent) {
    marginPercent = std::max(0.0f, std::min(25.0f, marginPercent));

    float margin = ws.width * marginPercent / 100.0f;

    margin = std::min(margin, ws.height * 0.49f);

    float availW = ws.width - 2.0f * margin;
    float availH = ws.height - 2.0f * margin;

    cellSize = std::min(availW / canvas.w, availH / canvas.h);
    cellSize = std::max(cellSize, 0.5f);

    float canvasPxW = canvas.w * cellSize;
    float canvasPxH = canvas.h * cellSize;

    originX = ws.x + (ws.width - canvasPxW) / 2.0f;
    originY = ws.y + (ws.height - canvasPxH) / 2.0f;
}

void View::zoomAt(float mx, float my, float factor) {
    float newSize = std::max(0.25f, std::min(cellSize * factor, 128.0f));
    float real = newSize / cellSize;
    originX = mx - (mx - originX) * real;
    originY = my - (my - originY) * real;
    cellSize = newSize;
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