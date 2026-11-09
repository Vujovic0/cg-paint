#include "CanvasView.h"

void CanvasView::fit() {
    marginPercent = std::max(0.0f, std::min(25.0f, marginPercent));

    float margin = workspace.width * marginPercent / 100.0f;

    margin = std::min(margin, workspace.height * 0.49f);

    float availW = workspace.width - 2.0f * margin;
    float availH = workspace.height - 2.0f * margin;

    cellSize = std::min(availW / canvas.w, availH / canvas.h);
    cellSize = std::max(cellSize, 0.5f);

    float canvasPxW = canvas.w * cellSize;
    float canvasPxH = canvas.h * cellSize;

    originX = workspace.x + (workspace.width - canvasPxW) / 2.0f;
    originY = workspace.y + (workspace.height - canvasPxH) / 2.0f;
}

void CanvasView::zoomAt(float mx, float my, float factor) {
    float newSize = std::max(0.01f, std::min(cellSize * factor, 1024.0f));
    float real = newSize / cellSize;
    originX = mx - (mx - originX) * real;
    originY = my - (my - originY) * real;
    cellSize = newSize;
}