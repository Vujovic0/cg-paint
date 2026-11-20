#include <vector>
#include "Vertex.h"
#include "Brush.h"
#include "constants.h"
#include "globals.h"
#include "drawBrush.h"

void drawBrush(GLFWwindow* window, unsigned int shader, Brush& cursor) {
    if (gbs::resizing || gbs::hoverEdges != 0) return;
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    std::vector<Vertex> verts = cursor.generateVertices(gbs::viewPtr->cellSize, mx, my);
    if (verts.empty() || verts.size() > constants::MAX_CURSOR_VERTS) return;

    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "uWindowMeasures"),
        (float)gbs::windowWidth, (float)gbs::windowHeight);

    glBindVertexArray(gbs::cursorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gbs::cursorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, verts.size() * sizeof(Vertex), verts.data());
    glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)verts.size());
    GLenum err = glGetError();
    if (err) printf("GL error 0x%x\n", err);
}