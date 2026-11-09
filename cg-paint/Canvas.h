#pragma once
#include <vector>
#include <stdexcept>

struct Color { unsigned char r, g, b, a; };
struct Vec2 { float x, y; };
struct Cell { int x, y; };
struct Workspace { float x, y, width, height; };

class Canvas {
private:
    bool inside(int x, int y) const {
        return x >= 0 && x < w && y >= 0 && y < h;
    }
public:
    int w, h;
    std::vector<Color> cells;
    Canvas(int w, int h) : w(w), h(h), cells(w* h, Color{ 255, 255, 255, 255 }) {}
    Color& at(int x, int y) { if (inside(x, y)) return cells[y * w + x]; else throw std::out_of_range("Canvas coordinates out of range");};
    void resize(int newW, int newH, int offX, int offY);
    void flipVertical();
    void flipHorizontal();
    void rotateCounterClockwise();
    void rotateClockwise();
};

class View {
public:
    float cellSize;
    float originX;
    float originY;
    float marginPercent;
    Workspace workspace;
    void fit(const Canvas& canvas);
    View(const Canvas& canvas, Workspace ws, float marginPercent) : workspace(ws), marginPercent(marginPercent) { fit(canvas); };
    Cell screenToCell(float sx, float sy) const { return Cell{ (int)floor((sx - originX) / cellSize), (int)floor((sy - originY) / cellSize) }; };
    Vec2 cellToScreen(int cx, int cy) const { return Vec2{ cx * cellSize + originX, cy * cellSize + originY }; };
    void zoomAt(float mx, float my, float factor);
    void pan(float dx, float dy) { originX += dx; originY += dy; };
};