#pragma once
#include <vector>
#include <stdexcept>
#include "structs.h"



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

