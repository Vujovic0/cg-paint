#include "paintAt.h"
#include "globals.h"
#include "vboHandling.h"
#include <vector>
#include <climits>
#include <algorithm>
#include <cstdlib>

static bool hasLastPaint = false;
static bool strokeActive = false;

static std::vector<unsigned int> paintedCells;
static unsigned int currentStroke = 0;

static std::vector<int> spanLow;
static std::vector<int> spanHigh;

static std::vector<int> stampLow;
static std::vector<int> stampHigh;
static int stampMinDy = 0;
static size_t stampCount = (size_t)-1;
static float stampRadius = -1.0f;

static void rebuildStampRows(const Brush& c) {
    if (c.stamp.size() == stampCount && c.radius == stampRadius) return;
    stampCount = c.stamp.size();
    stampRadius = c.radius;

    int minY = INT_MAX;
    int maxY = INT_MIN;
    for (const Cell& o : c.stamp) {
        minY = std::min(minY, o.y); 
        maxY = std::max(maxY, o.y); 
    }
    if (c.stamp.empty()) { 
        stampLow.clear(); stampHigh.clear(); stampMinDy = 0; return; 
    }

    stampMinDy = minY;
    stampLow.assign(maxY - minY + 1, INT_MAX);
    stampHigh.assign(maxY - minY + 1, INT_MIN);
    for (const Cell& o : c.stamp) {
        int i = o.y - minY;
        stampLow[i] = std::min(stampLow[i], o.x);
        stampHigh[i] = std::max(stampHigh[i], o.x);
    }
}

static Color blendColors(Color& src, Color& dst) {
    unsigned int a = src.a;
    unsigned int invA = 255 - a;

    return {
        (unsigned char)((src.r * a + dst.r * invA) / 255),
        (unsigned char)((src.g * a + dst.g * invA) / 255),
        (unsigned char)((src.b * a + dst.b * invA) / 255),
        255
    };
}

void paintAt(GLFWwindow* window, bool first) {
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    Cell cursorCell = gbs::viewPtr->screenToCell((float)mx, (float)my);

    Canvas& canvas = *gbs::canvasPtr;
    Brush& brush = *gbs::cursorPtr;

    if (paintedCells.size() != canvas.cells.size()) {
        paintedCells.assign(canvas.cells.size(), 0);
        currentStroke = 0;
    }

    if (first || !strokeActive) {
        ++currentStroke;

        if (currentStroke == 0) {
            std::fill(paintedCells.begin(), paintedCells.end(), 0);
            currentStroke = 1;
        }

        strokeActive = true;
    }

    if (paintedCells.size() != canvas.cells.size()) {
    paintedCells.assign(canvas.cells.size(), 0);
    currentStroke = 0;
}

    rebuildStampRows(brush);
    if ((int)spanLow.size() != canvas.h) {
        spanLow.assign(canvas.h, INT_MAX);
        spanHigh.assign(canvas.h, INT_MIN);
    }

    int minY = canvas.h;
    int maxY = -1;

    auto flushRow = [&](int yl, int lx0, int lx1) {
        for (size_t i = 0; i < stampLow.size(); ++i) {
            if (stampLow[i] > stampHigh[i]) continue;
            int y = yl + stampMinDy + (int)i;
            if ((unsigned)y >= (unsigned)canvas.h) continue;
            spanLow[y] = std::min(spanLow[y], lx0 + stampLow[i]);
            spanHigh[y] = std::max(spanHigh[y], lx1 + stampHigh[i]);
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);
        }
    };

    int lastCursorCellX;
    int lastCursorCellY;

    if (first || !hasLastPaint) { 
        lastCursorCellX = cursorCell.x; 
        lastCursorCellY = cursorCell.y; 
        hasLastPaint = true; 
    }
    else { 
        lastCursorCellX = gbs::lastPaintCellX; 
        lastCursorCellY = gbs::lastPaintCellY; 
    }
    int currentCursorCellX = cursorCell.x;
    int currentCursorCellY = cursorCell.y;

    int dx = std::abs(currentCursorCellX - lastCursorCellX);
    int sx = lastCursorCellX < currentCursorCellX ? 1 : -1;
    int dy = -std::abs(currentCursorCellY - lastCursorCellY);
    int sy = lastCursorCellY < currentCursorCellY ? 1 : -1;
    int err = dx + dy;

    int rowY = lastCursorCellY;
    int colLo = lastCursorCellX;
    int colHi = lastCursorCellX;
    while (true) {
        if (lastCursorCellY != rowY) {
            flushRow(rowY, colLo, colHi);
            rowY = lastCursorCellY; 
            colLo = colHi = lastCursorCellX;
        }
        else {
            colLo = std::min(colLo, lastCursorCellX);
            colHi = std::max(colHi, lastCursorCellX);
        }
        if (lastCursorCellX == currentCursorCellX && lastCursorCellY == currentCursorCellY) 
            break;
        int e2 = 2 * err;
        if (e2 >= dy) { 
            err += dy; 
            lastCursorCellX += sx; 
        }
        if (e2 <= dx) { 
            err += dx; 
            lastCursorCellY += sy; 
        }
    }
    flushRow(rowY, colLo, colHi);

    gbs::lastPaintCellX = cursorCell.x;
    gbs::lastPaintCellY = cursorCell.y;

    if (maxY < 0) return;

    Color color = (brush.isBrush) ? brush.color : Color{ 255,255,255,255 };
    glBindBuffer(GL_ARRAY_BUFFER, gbs::colorVBO);
    for (int y = minY; y <= maxY; ++y) {
        int lo = std::max(spanLow[y], 0);
        int hi = std::min(spanHigh[y], canvas.w - 1);
        spanLow[y] = INT_MAX;
        spanHigh[y] = INT_MIN;
        if (lo > hi) continue;

        size_t idx = (size_t)y * canvas.w + lo;
        size_t n = (size_t)(hi - lo + 1);
        for (size_t i = 0; i < n; ++i) {
            size_t cellIdx = idx + i;

            if (paintedCells[cellIdx] == currentStroke)
                continue;

            paintedCells[cellIdx] = currentStroke;

            canvas.cells[cellIdx] = blendColors(
                color,
                canvas.cells[cellIdx]
            );
        }
        glBufferSubData(GL_ARRAY_BUFFER, idx * sizeof(Color),
            n * sizeof(Color), &canvas.cells[idx]);
    }
}


void endStroke() {
    strokeActive = false;
    hasLastPaint = false;
}