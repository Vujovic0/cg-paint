#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "globals.h"
#include "vboHandling.h"
#include "resize.h"
#include "edge.h"
#include <cmath>
#include "Vertex.h"

void computeResize(float mx, float my) {
    Canvas& cv = *gbs::canvasPtr;
    CanvasView& v = *gbs::viewPtr;
    float cs = v.cellSize;
    Cell c = v.screenToCell(mx + cs / 2.0f, my + cs / 2.0f);

    int newW = cv.w, newH = cv.h, offX = 0, offY = 0;

    if (gbs::resizeEdges & EDGE_R) newW = std::max(1, std::min(c.x, gbs::maxCanvasSize));
    if (gbs::resizeEdges & EDGE_L) {
        newW = std::max(1, std::min(cv.w - c.x, gbs::maxCanvasSize));
        offX = cv.w - newW;
    }
    if (gbs::resizeEdges & EDGE_B) newH = std::max(1, std::min(c.y, gbs::maxCanvasSize));
    if (gbs::resizeEdges & EDGE_T) {
        newH = std::max(1, std::min(cv.h - c.y, gbs::maxCanvasSize));
        offY = cv.h - newH;
    }

    gbs::pendingW = newW;
    gbs::pendingH = newH;
    gbs::pendingOffX = offX;
    gbs::pendingOffY = offY;
}

void finishResize() {
    Canvas& cv = *gbs::canvasPtr;
    CanvasView& v = *gbs::viewPtr;

    if (gbs::pendingW != cv.w || gbs::pendingH != cv.h) {
        cv.resize(gbs::pendingW, gbs::pendingH, gbs::pendingOffX, gbs::pendingOffY);
        v.originX += gbs::pendingOffX * v.cellSize;
        v.originY += gbs::pendingOffY * v.cellSize;
        reallocColors();
    }
}

void drawResizePreview(unsigned int shader) {
    float cs = gbs::viewPtr->cellSize;
    float x0 = gbs::viewPtr->originX + gbs::pendingOffX * cs;
    float y0 = gbs::viewPtr->originY + gbs::pendingOffY * cs;
    float x1 = x0 + gbs::pendingW * cs;
    float y1 = y0 + gbs::pendingH * cs;
    Vertex verts[] = { 
        {x0,y0, 210, 210, 210, 255},
        {x1,y0, 210, 210, 210, 255}, 
        {x1,y1, 210, 210, 210, 255},  
        {x0,y1 ,210, 210, 210, 255}
    };

    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "uWindowMeasures"), gbs::windowWidth, gbs::windowHeight);
    glBindVertexArray(gbs::previewVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gbs::previewVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}