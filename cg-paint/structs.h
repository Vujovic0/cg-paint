#pragma once

struct Color { 
    unsigned char r, g, b, a; 
    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};
struct Vec2 { float x, y; };
struct Workspace { float x, y, width, height; };
struct Cell { int x, y; };
