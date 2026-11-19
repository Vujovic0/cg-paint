#include "Canvas.h"
#include "globals.h"
#include "vboHandling.h"
#include "constants.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include "Vertex.h"
#include <map>

void reallocColors() {
    glBindBuffer(GL_ARRAY_BUFFER, gbs::colorVBO);
    glBufferData(GL_ARRAY_BUFFER, gbs::canvasPtr->cells.size() * sizeof(Color),
        gbs::canvasPtr->cells.data(), GL_DYNAMIC_DRAW);
}

void populateVBO(Canvas& canvas) {
    glGenVertexArrays(1, &gbs::canvasVAO);
    glBindVertexArray(gbs::canvasVAO);

    float quad[] = { 0,0,  1,0,  0,1, 1,1};
    glGenBuffers(1, &gbs::quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gbs::quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &gbs::colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gbs::colorVBO);
    glBufferData(GL_ARRAY_BUFFER, canvas.cells.size() * sizeof(Color),
        canvas.cells.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Color), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glGenVertexArrays(1, &gbs::previewVAO);
    glBindVertexArray(gbs::previewVAO);
    glGenBuffers(1, &gbs::previewVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gbs::previewVBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, r));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &gbs::cursorVAO);
    glBindVertexArray(gbs::cursorVAO);
    glGenBuffers(1, &gbs::cursorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gbs::cursorVBO);
    glBufferData(GL_ARRAY_BUFFER, constants::MAX_CURSOR_VERTS * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, r));
    glEnableVertexAttribArray(1);
}

void uploadColors(unsigned int colorVBO, const Canvas& canvas) {
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        canvas.cells.size() * sizeof(Color),
        canvas.cells.data());
    gbs::minimapPtr->reset();
}

void uploadCells(const std::vector<Cell>& changed) {
    if (changed.empty()) return;
    Canvas& cv = *gbs::canvasPtr;

    std::map<int, std::pair<int, int>> rows;
    for (const Cell& c : changed) {
        auto it = rows.find(c.y);
        if (it == rows.end()) {
            rows[c.y] = { c.x, c.x };
        }
        else {
            it->second.first = std::min(it->second.first, c.x);
            it->second.second = std::max(it->second.second, c.x);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, gbs::colorVBO);
    for (const auto& [y, range] : rows) {
        size_t first = (size_t)y * cv.w + range.first;
        size_t count = (size_t)(range.second - range.first + 1);
        glBufferSubData(GL_ARRAY_BUFFER,
            first * sizeof(Color),
            count * sizeof(Color),
            &cv.cells[first]);
    }
}