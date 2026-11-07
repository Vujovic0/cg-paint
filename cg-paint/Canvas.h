#pragma once
#include <vector>
#include <stdexcept>

struct Color { float r, g, b, a; };
struct Vec2 { float x, y; };
struct Cell { int x, y; };

class Canvas {
private:
    int w, h;
    std::vector<Color> cells;
    bool inside(int x, int y) const {
        return x >= 0 && x < w && y >= 0 && y < h;
    }
public:
    Canvas(int w, int h) : w(w), h(h), cells(w* h, Color{ 0.0f, 0.0f, 0.0f, 1.0f }) {}
    ~Canvas();
    Color& at(int x, int y) { if (inside(x, y)) return cells[y * w + x]; else throw std::out_of_range("Canvas coordinates out of range");};
    void resize(int newW, int newH);
};

class View {
private:
    float cellSize;
    float originX;
    float originY;
public:
    View() : cellSize(16.0f), originX(0.0f), originY(0.0f) {}
    Cell screenToCell(float sx, float sy) const { return Cell{ (int)floor((sx - originX) / cellSize), (int)floor((sy - originY) / cellSize) }; };
    Vec2 cellToScreen(int cx, int cy) const { return Vec2{ cx * cellSize + originX, cy * cellSize + originY }; };
    void zoomIn() { cellSize = std::min(cellSize * 1.1f, 128.0f); };
    void zoomOut() { cellSize = std::max(cellSize / 1.1f, 2.0f); };
    void pan(float dx, float dy) { originX += dx; originY += dy; };
};