#pragma once
#include <string>
#include <cstddef>
#include "structs.h"

class Renderer {
public:
    Renderer(const std::string& fontPath, int pixelSize, unsigned int basicShader, unsigned int textShader);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void fillRect(int x, int y, int w, int h, Color c);
    void drawText(const std::string& s, int x, int baselineY, Color c);
    int  measureText(const std::string& s) const;
    int  textHeight() const { return ascent; }

private:
    Glyph glyphs[128];
    int ascent = 0;
    unsigned int basicShader = 0;
    unsigned int textShader = 0;
    unsigned int textVAO = 0;
    unsigned int textVBO = 0;
};