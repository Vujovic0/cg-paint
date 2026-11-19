#include "Cursor.h"
#include <cmath>
#include "constants.h"

constexpr int CIRCLE_RADIUS_SPIKE_TOLERANCE = 5;

std::vector<Vertex> Cursor::generateVertices(float cellSize, double mx, double my) {
    switch (shape) {
    case CursorShape::CIRCLE:   return createCircle(cellSize, mx, my);
    case CursorShape::TRIANGLE: return createTriangle(cellSize, mx, my);
    case CursorShape::SQUARE:   return createSquare(cellSize, mx, my);
    }
    return {};
}

std::vector<Vertex> Cursor::createCircle(float cellSize, double mx, double my) {
    const float r = cellSize * radius;
    const float PI = 3.14159265f;
    const float cx = (float)mx, cy = (float)my;

    int N = std::max(12, std::min((int)(r * 0.5f), constants::MAX_SEGMENTS));

    Vertex center = { cx, cy, color.r, color.g, color.b, color.a };
    std::vector<Vertex> v;
    v.reserve(N);
    for (int i = 0; i < N; i++) {
        float a = 2.0f * PI * i / N;
        v.push_back({ cx + cosf(a) * r, cy + sinf(a) * r, 0, 0, 0, 255 });
    }
    return v;
}

std::vector<Vertex> Cursor::createTriangle(float cellSize, double mx, double my) {
    const float h = cellSize * radius;
    const float cx = (float)mx, cy = (float)my;
    return {
        { cx,     cy - h, 0, 0, 0, 255 },
        { cx - h, cy + h, 0, 0, 0, 255 },
        { cx + h, cy + h, 0, 0, 0, 255 }
    };
}

std::vector<Vertex> Cursor::createSquare(float cellSize, double mx, double my) {
    const float h = cellSize * radius;
    const float cx = (float)mx, cy = (float)my;

    Vertex tl = { cx - h, cy - h, 0, 0, 0, 255 };
    Vertex tr = { cx + h, cy - h, 0, 0, 0, 255 };
    Vertex br = { cx + h, cy + h, 0, 0, 0, 255 };
    Vertex bl = { cx - h, cy + h, 0, 0, 0, 255 };

    return { tl, tr, br, bl};
}

void Cursor::rebuildStamp() {
    stamp.clear();
    int radius = (int)ceilf(this->radius);
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            bool in = false;
            switch (shape) {
            case CursorShape::CIRCLE:
                if (radius < CIRCLE_RADIUS_SPIKE_TOLERANCE)
                    in = dx * dx + dy * dy <= radius * radius; 
                else
                    in = dx * dx + dy * dy < radius * radius;
                break;
            case CursorShape::SQUARE:
                in = abs(dx) <= radius && abs(dy) <= radius; 
                break;
            case CursorShape::TRIANGLE:
                in = abs(dx) <= (dy + radius) * 0.5f; 
                break;
            }
            if (in) stamp.push_back(Cell{ dx, dy });
        }
    }
}