#include "paintAt.h"
#include "globals.h"
#include "vboHandling.h"

void paintAt(GLFWwindow* window) {
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    Cell center = gbs::viewPtr->screenToCell((float)mx, (float)my);

    Canvas& cv = *gbs::canvasPtr;
    Cursor& c = *gbs::cursorPtr;

    std::vector<Cell> changed;
    changed.reserve(c.stamp.size());

    for (const Cell& o : c.stamp) {
        int x = center.x + o.x;
        int y = center.y + o.y;
        if (x < 0 || x >= cv.w || y < 0 || y >= cv.h) continue;

        cv.cells[(size_t)y * cv.w + x] = c.color;
        changed.push_back(Cell{ x, y });
    }

    uploadCells(changed);
}