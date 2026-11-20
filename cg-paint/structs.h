#pragma once

struct Vec2 { float x, y; };
struct Workspace { float x, y, width, height; };
struct Cell { int x, y; };


struct Color { 
    unsigned char r, g, b, a; 
    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};

struct RectVertex {
    float x, y;
    unsigned char r, g, b, a;
};

struct Glyph {
    unsigned int texture = 0;
    int width = 0, height = 0;
    int bearingX = 0, bearingY = 0;
    int advance = 0;
};
