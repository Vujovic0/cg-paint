#include "paintAt.h"
#include "globals.h"
#include "vboHandling.h"
#include <vector>
#include <climits>
#include <algorithm>
#include <cstdlib>

static bool hasLastPaint = false;

// raspon pokrivenih x po redu platna
static std::vector<int> spanLo, spanHi;

// raspon stampa po njegovom redu (keširano)
static std::vector<int> stLo, stHi;
static int stMinDy = 0;
static size_t stCount = (size_t)-1;
static float stSize = -1.0f;

static void rebuildStampRows(const Cursor& c) {
    if (c.stamp.size() == stCount && c.size == stSize) return;
    stCount = c.stamp.size();
    stSize = c.size;

    int mn = INT_MAX, mx = INT_MIN;
    for (const Cell& o : c.stamp) { mn = std::min(mn, o.y); mx = std::max(mx, o.y); }
    if (c.stamp.empty()) { stLo.clear(); stHi.clear(); stMinDy = 0; return; }

    stMinDy = mn;
    stLo.assign(mx - mn + 1, INT_MAX);
    stHi.assign(mx - mn + 1, INT_MIN);
    for (const Cell& o : c.stamp) {
        int i = o.y - mn;
        stLo[i] = std::min(stLo[i], o.x);
        stHi[i] = std::max(stHi[i], o.x);
    }
}

void paintAt(GLFWwindow* window, bool first) {
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    Cell cur = gbs::viewPtr->screenToCell((float)mx, (float)my);

    Canvas& cv = *gbs::canvasPtr;
    Cursor& c = *gbs::cursorPtr;

    rebuildStampRows(c);
    if ((int)spanLo.size() != cv.h) {
        spanLo.assign(cv.h, INT_MAX);
        spanHi.assign(cv.h, INT_MIN);
    }

    int minY = cv.h, maxY = -1;

    // doda pokrivenost jednog reda linije [lx0,lx1] na yl
    auto flushRow = [&](int yl, int lx0, int lx1) {
        for (size_t i = 0; i < stLo.size(); ++i) {
            if (stLo[i] > stHi[i]) continue;
            int y = yl + stMinDy + (int)i;
            if ((unsigned)y >= (unsigned)cv.h) continue;
            spanLo[y] = std::min(spanLo[y], lx0 + stLo[i]);
            spanHi[y] = std::max(spanHi[y], lx1 + stHi[i]);
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);
        }
        };

    int x0, y0;
    if (first || !hasLastPaint) { x0 = cur.x; y0 = cur.y; hasLastPaint = true; }
    else { x0 = gbs::lastPaintX; y0 = gbs::lastPaintY; }
    int x1 = cur.x, y1 = cur.y;

    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    int rowY = y0, rowLo = x0, rowHi = x0;
    while (true) {
        if (y0 != rowY) {                 // prešli smo u novi red linije
            flushRow(rowY, rowLo, rowHi);
            rowY = y0; rowLo = rowHi = x0;
        }
        else {
            rowLo = std::min(rowLo, x0);
            rowHi = std::max(rowHi, x0);
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
    flushRow(rowY, rowLo, rowHi);

    gbs::lastPaintX = cur.x;
    gbs::lastPaintY = cur.y;

    if (maxY < 0) return;

    glBindBuffer(GL_ARRAY_BUFFER, gbs::colorVBO);
    for (int y = minY; y <= maxY; ++y) {
        int lo = std::max(spanLo[y], 0);
        int hi = std::min(spanHi[y], cv.w - 1);
        spanLo[y] = INT_MAX;
        spanHi[y] = INT_MIN;
        if (lo > hi) continue;

        size_t idx = (size_t)y * cv.w + lo;
        size_t n = (size_t)(hi - lo + 1);
        std::fill(cv.cells.begin() + idx, cv.cells.begin() + idx + n, c.color);
        glBufferSubData(GL_ARRAY_BUFFER, idx * sizeof(Color),
            n * sizeof(Color), &cv.cells[idx]);
    }
}