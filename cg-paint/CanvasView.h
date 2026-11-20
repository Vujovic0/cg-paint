#pragma once
#include "Canvas.h"


class CanvasView {
public:
    float cellSize;
    float originX;
    float originY;
    float marginPercent;
    const Workspace& workspace;
    const Canvas& canvas;
    void fit();
    CanvasView(const Canvas& canvas, const Workspace& ws, float marginPercent) : canvas(canvas), workspace(ws), marginPercent(marginPercent) { fit(); };
    Cell screenToCell(float sx, float sy) const { return Cell{ (int)floor((sx - originX) / cellSize), (int)floor((sy - originY) / cellSize) }; };
    Vec2 cellToScreen(int cx, int cy) const { return Vec2{ cx * cellSize + originX, cy * cellSize + originY }; };
    void zoomAt(float mx, float my, float factor);
    void pan(float dx, float dy) { originX += dx; originY += dy; };
    bool isCursorOn(int mx, int my);
};