#include "callbacks.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "globals.h"
#include "vboHandling.h"
#include "resize.h"
#include "edge.h"
#include "constants.h"
#include <algorithm>
#include "paintAt.h"

static constexpr Color palette[] = {
    Colors::BLACK,
    Colors::RED,
    Colors::GREEN,
    Colors::BLUE
};

static bool wasOnCanvas = false;
static constexpr size_t paletteSize = sizeof(palette) / sizeof(palette[0]);

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            std::cout << "User pressed escape" << std::endl;
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (gbs::toolbarPtr && gbs::toolbarPtr->hasFocus()) {
            if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
                gbs::toolbarPtr->onBackspace();
            return; 
        }
        if (key == GLFW_KEY_1) { gbs::cursorPtr->shape = CursorShape::CIRCLE;   gbs::cursorPtr->rebuildStamp(); }
        if (key == GLFW_KEY_2) { gbs::cursorPtr->shape = CursorShape::TRIANGLE; gbs::cursorPtr->rebuildStamp(); }
        if (key == GLFW_KEY_3) { gbs::cursorPtr->shape = CursorShape::SQUARE;   gbs::cursorPtr->rebuildStamp(); }
        if (key == GLFW_KEY_H) { gbs::canvasPtr->flipHorizontal(); uploadColors(gbs::colorVBO, *gbs::canvasPtr); }
        if (key == GLFW_KEY_V) { gbs::canvasPtr->flipVertical();   uploadColors(gbs::colorVBO, *gbs::canvasPtr); }
        if (key == GLFW_KEY_R) {
            if (mods & GLFW_MOD_SHIFT)
                gbs::canvasPtr->rotateCounterClockwise();
            else
                gbs::canvasPtr->rotateClockwise();
            uploadColors(gbs::colorVBO, *gbs::canvasPtr);
            gbs::viewPtr->fit();
        }
        if (key == GLFW_KEY_S) {
            size_t randomIndex = std::rand() % paletteSize;
            gbs::cursorPtr->color = palette[randomIndex];
        }
        if (key == GLFW_KEY_E) {
            gbs::cursorPtr->color = Colors::WHITE;
        }
        if (key == GLFW_KEY_BACKSPACE) {
            gbs::toolbarPtr->onBackspace();
        }
    }
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            gbs::panning = true;
            glfwGetCursorPos(window, &gbs::lastMouseX, &gbs::lastMouseY);
        }
        else if (action == GLFW_RELEASE) {
            gbs::panning = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            if (gbs::toolbarPtr) gbs::toolbarPtr->onClick((int)mx, (int)my);
            int e = detectEdges((float)mx, (float)my);
            if (e) {
                gbs::resizing = true;
                gbs::resizeEdges = e;
                gbs::pendingW = gbs::canvasPtr->w;
                gbs::pendingH = gbs::canvasPtr->h;
                gbs::pendingOffX = 0;
                gbs::pendingOffY = 0;
            }
            else {
                gbs::painting = true;
                paintAt(window, true);
            }
        }
        else if (action == GLFW_RELEASE) {
            if (gbs::resizing)
            {
                finishResize();
                gbs::minimapPtr->reset();
            }

            gbs::resizing = false;
            gbs::resizeEdges = 0;
            gbs::painting = false;
            endStroke();
        }
    }
}

void charCallback(GLFWwindow*, unsigned int codepoint) {
    if (codepoint < 128) gbs::toolbarPtr->onChar((char)codepoint);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!gbs::viewPtr)
        return;

    bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;

    if (!ctrl) {
        gbs::cursorPtr->radius *= powf(1.1f, (float)yoffset);
        gbs::cursorPtr->radius = std::clamp(gbs::cursorPtr->radius,
            constants::MIN_CURSOR_SIZE, constants::MAX_CURSOR_SIZE);
        gbs::cursorPtr->rebuildStamp();
        return;
    }

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    gbs::viewPtr->zoomAt((float)mx, (float)my, powf(1.2f, (float)yoffset));
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    if (!gbs::viewPtr)
        return;
    if (gbs::resizing) {
        computeResize((float)x, (float)y);
    }
    else if (gbs::panning) {
        gbs::viewPtr->pan((float)(x - gbs::lastMouseX), (float)(y - gbs::lastMouseY));
        gbs::lastMouseX = x;
        gbs::lastMouseY = y;
    }
    else {
        updateCursor(window, detectEdges((float)x, (float)y), x, y);
        if (gbs::painting) paintAt(window, false);
    }
}

int detectEdges(float mx, float my) {
    Cell a = gbs::viewPtr->screenToCell(mx - gbs::grabPx, my - gbs::grabPx);
    Cell b = gbs::viewPtr->screenToCell(mx + gbs::grabPx, my + gbs::grabPx);
    int w = gbs::canvasPtr->w, h = gbs::canvasPtr->h;

    bool overX = b.x >= 0 && a.x <= w - 1;
    bool overY = b.y >= 0 && a.y <= h - 1;

    int e = 0;
    if (overY) {
        if (a.x <= w - 1 && b.x >= w) e |= EDGE_R;
        else if (a.x < 0 && b.x >= 0) e |= EDGE_L;
    }
    if (overX) {
        if (a.y <= h - 1 && b.y >= h) e |= EDGE_B;
        else if (a.y < 0 && b.y >= 0) e |= EDGE_T;
    }
    return e;
}

void updateCursor(GLFWwindow* window, int edges, double mx, double my) {
    bool onCanvas = gbs::viewPtr->isCursorIn(mx, my);
    if (edges == gbs::hoverEdges && onCanvas == wasOnCanvas) return;
    gbs::hoverEdges = edges;
    wasOnCanvas = onCanvas;

    GLFWcursor* c = nullptr;
    bool l = edges & EDGE_L, r = edges & EDGE_R, t = edges & EDGE_T, b = edges & EDGE_B;
    if ((l && t) || (r && b))       c = gbs::curNWSE;
    else if ((r && t) || (l && b))  c = gbs::curNESW;
    else if (l || r)                c = gbs::curH;
    else if (t || b)                c = gbs::curV;

    if (edges == 0) {
        if (!onCanvas) {
            glfwSetCursor(window, nullptr);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            return;
        }
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursor(window, c);
    }
}