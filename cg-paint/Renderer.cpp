#include "Renderer.h"
#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdexcept>
#include "shaderUtil.h"
#include "globals.h"

Renderer::Renderer(const std::string& fontPath, int pixelSize, unsigned int basicShader, unsigned int textShader) {
    this->textShader = textShader;
    this->basicShader = basicShader;

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) throw std::runtime_error("FreeType init failed");
    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        FT_Done_FreeType(ft);
        throw std::runtime_error("Font failed to load: " + fontPath);
    }
    FT_Set_Pixel_Sizes(face, 0, pixelSize);
    ascent = (int)(face->size->metrics.ascender >> 6);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned char c = 32; c < 127; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        FT_GlyphSlot g = face->glyph;

        Glyph& gl = glyphs[c];
        gl.width = g->bitmap.width;
        gl.height = g->bitmap.rows;
        gl.bearingX = g->bitmap_left;
        gl.bearingY = g->bitmap_top;
        gl.advance = (int)(g->advance.x >> 6);

        if (gl.width == 0 || gl.height == 0) continue; 

        glGenTextures(1, &gl.texture);
        glBindTexture(GL_TEXTURE_2D, gl.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, gl.width, gl.height, 0,
            GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindVertexArray(0);
}

Renderer::~Renderer() {
    for (auto& g : glyphs)
        if (g.texture) glDeleteTextures(1, &g.texture);
    glDeleteBuffers(1, &textVBO);
    glDeleteVertexArrays(1, &textVAO);
    glDeleteProgram(textShader);
}

void Renderer::drawText(const std::string& s, int x, int baselineY, Color c) {
    glUseProgram(textShader);
    glUniform2f(glGetUniformLocation(textShader, "uWindowMeasures"),
        (float)gbs::windowWidth, (float)gbs::windowHeight);
    glUniform4f(glGetUniformLocation(textShader, "uColor"),
        c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
    glUniform1i(glGetUniformLocation(textShader, "uGlyph"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);

    float cx = (float)x;
    for (unsigned char ch : s) {
        if (ch < 32 || ch >= 127) continue;
        const Glyph& g = glyphs[ch];

        if (g.texture) {
            float xpos = cx + g.bearingX;
            float ypos = (float)(baselineY - g.bearingY);
            float w = (float)g.width, h = (float)g.height;

            float v[6][4] = {
                { xpos,     ypos,     0, 0 },
                { xpos,     ypos + h, 0, 1 },
                { xpos + w, ypos + h, 1, 1 },

                { xpos,     ypos,     0, 0 },
                { xpos + w, ypos + h, 1, 1 },
                { xpos + w, ypos,     1, 0 }
            };
            glBindTexture(GL_TEXTURE_2D, g.texture);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v), v);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        cx += g.advance;
    }
}

int Renderer::measureText(const std::string& s) const {
    int w = 0;
    for (unsigned char ch : s)
        if (ch >= 32 && ch < 127) w += glyphs[ch].advance;
    return w;
}

void Renderer::fillRect(int x, int y, int w, int h, Color c) {
    float X = (float)x, Y = (float)y, W = (float)w, H = (float)h;
    RectVertex v[4] = {
        { X,     Y,     c.r, c.g, c.b, c.a },
        { X + W, Y,     c.r, c.g, c.b, c.a },
        { X,     Y + H, c.r, c.g, c.b, c.a },
        { X + W, Y + H, c.r, c.g, c.b, c.a },
    };
    glUseProgram(basicShader);
    glUniform2f(glGetUniformLocation(basicShader, "uWindowMeasures"),
        (float)gbs::windowWidth, (float)gbs::windowHeight);
    glBindVertexArray(gbs::rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gbs::rectVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v), v);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}