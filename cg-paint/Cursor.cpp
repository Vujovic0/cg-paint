#include "Cursor.h"
#include <cmath>
#include "constants.h"

std::vector<Vertex> Cursor::generateVertices(float cellSize, double mx, double my) {
    switch (shape) {
    case CursorShape::CIRCLE:   return createCircle(cellSize, mx, my);
    case CursorShape::TRIANGLE: return createTriangle(cellSize, mx, my);
    case CursorShape::SQUARE:   return createSquare(cellSize, mx, my);
    }
    return {};
}

std::vector<Vertex> Cursor::createCircle(float cellSize, double mx, double my) {
    const float r = cellSize * size;
    const float PI = 3.14159265f;
    const float cx = (float)mx, cy = (float)my;

    int N = std::max(12, std::min((int)(r * 0.5f), constants::MAX_SEGMENTS));

    Vertex center = { cx, cy, color.r, color.g, color.b, color.a };
    std::vector<Vertex> v;
    v.reserve(N);
    for (int i = 0; i < N; i++) {
        float a = 2.0f * PI * i / N;
        v.push_back({ cx + cosf(a) * r, cy + sinf(a) * r, color.r, color.g, color.b, color.a });
    }
    return v;
}

std::vector<Vertex> Cursor::createTriangle(float cellSize, double mx, double my) {
    const float h = cellSize * size;
    const float cx = (float)mx, cy = (float)my;
    return {
        { cx,     cy - h, color.r, color.g, color.b, color.a },
        { cx - h, cy + h, color.r, color.g, color.b, color.a },
        { cx + h, cy + h, color.r, color.g, color.b, color.a }
    };
}

std::vector<Vertex> Cursor::createSquare(float cellSize, double mx, double my) {
    const float h = cellSize * size;
    const float cx = (float)mx, cy = (float)my;

    Vertex tl = { cx - h, cy - h, color.r, color.g, color.b, color.a };
    Vertex tr = { cx + h, cy - h, color.r, color.g, color.b, color.a };
    Vertex br = { cx + h, cy + h, color.r, color.g, color.b, color.a };
    Vertex bl = { cx - h, cy + h, color.r, color.g, color.b, color.a };

    return { tl, tr, br, bl};
}

void Cursor::rebuildStamp() {
    stamp.clear();
    int R = (int)ceilf(size);
    for (int dy = -R; dy <= R; dy++) {
        for (int dx = -R; dx <= R; dx++) {
            bool in = false;
            switch (shape) {
            case CursorShape::CIRCLE:
                in = dx * dx + dy * dy <= size * size; break;
            case CursorShape::SQUARE:
                in = abs(dx) <= size && abs(dy) <= size; break;
            case CursorShape::TRIANGLE:
                in = abs(dx) <= (dy + R) * 0.5f; break;
            }
            if (in) stamp.push_back(Cell{ dx, dy });
        }
    }
}