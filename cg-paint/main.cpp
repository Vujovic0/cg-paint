#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>
#include <thread>
#include "shaderUtil.h"
#include "Canvas.h"


float uAspectRatio = 1.0f;
float windowWidth;
float windowHeight;

unsigned int VAO;
unsigned int quadVBO;
unsigned int colorVBO;

View* viewPtr = nullptr;
Canvas* canvasPtr = nullptr;
bool panning = false;
double lastMouseX = 0, lastMouseY = 0;

enum { EDGE_L = 1, EDGE_R = 2, EDGE_T = 4, EDGE_B = 8 };
const float grabPx = 8.0f;
const int maxCanvasSize = 4096;

bool resizing = false;
int resizeEdges = 0;
int hoverEdges = -1;

GLFWcursor* curH = nullptr;
GLFWcursor* curV = nullptr;
GLFWcursor* curNWSE = nullptr;
GLFWcursor* curNESW = nullptr;

int pendingW = 0, pendingH = 0, pendingOffX = 0, pendingOffY = 0;
unsigned int previewVAO, previewVBO;

const auto frameDuration = std::chrono::microseconds(1000000 / 75);

void reallocColors() {
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, canvasPtr->cells.size() * sizeof(Color),
        canvasPtr->cells.data(), GL_DYNAMIC_DRAW);
}

int detectEdges(float mx, float my) {
    Cell a = viewPtr->screenToCell(mx - grabPx, my - grabPx);
    Cell b = viewPtr->screenToCell(mx + grabPx, my + grabPx);
    int w = canvasPtr->w, h = canvasPtr->h;

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

void updateCursor(GLFWwindow* window, int edges) {
    if (edges == hoverEdges) return;
    hoverEdges = edges;

    GLFWcursor* c = nullptr;
    bool l = edges & EDGE_L, r = edges & EDGE_R, t = edges & EDGE_T, b = edges & EDGE_B;
    if ((l && t) || (r && b)) c = curNWSE;
    else if ((r && t) || (l && b)) c = curNESW;
    else if (l || r) c = curH;
    else if (t || b) c = curV;

    glfwSetCursor(window, c);
}

void computeResize(float mx, float my) {
    Canvas& cv = *canvasPtr;
    View& v = *viewPtr;
    float cs = v.cellSize;
    Cell c = v.screenToCell(mx + cs / 2.0f, my + cs / 2.0f);

    int newW = cv.w, newH = cv.h, offX = 0, offY = 0;

    if (resizeEdges & EDGE_R) newW = std::max(1, std::min(c.x, maxCanvasSize));
    if (resizeEdges & EDGE_L) {
        newW = std::max(1, std::min(cv.w - c.x, maxCanvasSize));
        offX = cv.w - newW;
    }
    if (resizeEdges & EDGE_B) newH = std::max(1, std::min(c.y, maxCanvasSize));
    if (resizeEdges & EDGE_T) {
        newH = std::max(1, std::min(cv.h - c.y, maxCanvasSize));
        offY = cv.h - newH;
    }

    pendingW = newW;
    pendingH = newH;
    pendingOffX = offX;
    pendingOffY = offY;
}

void finishResize() {
    Canvas& cv = *canvasPtr;
    View& v = *viewPtr;

    if (pendingW != cv.w || pendingH != cv.h) {
        cv.resize(pendingW, pendingH, pendingOffX, pendingOffY);
        v.originX += pendingOffX * v.cellSize;
        v.originY += pendingOffY * v.cellSize;
        reallocColors();
    }
}

void drawResizePreview(unsigned int shader) {
    float cs = viewPtr->cellSize;
    float x0 = viewPtr->originX + pendingOffX * cs;
    float y0 = viewPtr->originY + pendingOffY * cs;
    float x1 = x0 + pendingW * cs;
    float y1 = y0 + pendingH * cs;
    float verts[] = { x0,y0,  x1,y0,  x1,y1,  x0,y1 };

    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "uWindowMeasures"), windowWidth, windowHeight);
    glBindVertexArray(previewVAO);
    glBindBuffer(GL_ARRAY_BUFFER, previewVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

int endProgram(std::string message) {
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}


void uploadColors(unsigned int colorVBO, const Canvas& canvas) {
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        canvas.cells.size() * sizeof(Color),
        canvas.cells.data());
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        std::cout << "User pressed escape" << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_H) { canvasPtr->flipHorizontal(); uploadColors(colorVBO, *canvasPtr); }
        if (key == GLFW_KEY_V) { canvasPtr->flipVertical();   uploadColors(colorVBO, *canvasPtr); }
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        if (mods & GLFW_MOD_SHIFT) canvasPtr->rotateCounterClockwise();
        else                       canvasPtr->rotateClockwise();
        uploadColors(colorVBO, *canvasPtr);
        viewPtr->fit(*canvasPtr);
    }
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            panning = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
        else if (action == GLFW_RELEASE) {
            panning = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            int e = detectEdges((float)mx, (float)my);
            if (e) {
                resizing = true;
                resizeEdges = e;
                pendingW = canvasPtr->w;  pendingH = canvasPtr->h;
                pendingOffX = 0;          pendingOffY = 0;
            }
        }
        else if (action == GLFW_RELEASE) {
            if (resizing) finishResize();
            resizing = false;
            resizeEdges = 0;
        }
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!viewPtr) return;
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    viewPtr->zoomAt((float)mx, (float)my, powf(1.2f, (float)yoffset));
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    if (!viewPtr) return;
    if (resizing) {
        computeResize((float)x, (float)y);
    }
    else if (panning) {
        viewPtr->pan((float)(x - lastMouseX), (float)(y - lastMouseY));
        lastMouseX = x;
        lastMouseY = y;
    }
    else {
        updateCursor(window, detectEdges((float)x, (float)y));
    }
}

GLFWwindow* createFullscreenWindow() {

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    uAspectRatio = (float)mode->width / mode->height;
    windowWidth = mode->width;
    windowHeight = mode->height;

    return glfwCreateWindow(mode->width, mode->height, "MiniPaint", monitor, NULL);
}

void populateVBO(Canvas &canvas) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    float quad[] = { 0,0,  1,0,  0,1,   1,0,  1,1,  0,1 };
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, canvas.cells.size() * sizeof(Color),
        canvas.cells.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Color), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);  

    glGenVertexArrays(1, &previewVAO);
    glBindVertexArray(previewVAO);
    glGenBuffers(1, &previewVBO);
    glBindBuffer(GL_ARRAY_BUFFER, previewVBO);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    curH = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    curV = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    curNWSE = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    curNESW = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);

    GLFWwindow* window = createFullscreenWindow();
    if (window == NULL) return endProgram("Window failed to be created");
    
    glfwMakeContextCurrent(window);
    glfwFocusWindow(window);

    glfwSetScrollCallback(window, scrollCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);

    if (glewInit() != GLEW_OK) return endProgram("GLEW failed to initialize");

    unsigned int canvasShader = createShader("canvas.vert", "canvas.frag");
    unsigned int previewShader = createShader("preview.vert", "preview.frag");

    Canvas canvas(3, 2);
    canvas.at(0, 0) = Color{ 1, 0, 0, 1 };

    Workspace workspace = Workspace{ 0.0f, 0.0f, windowWidth, windowHeight };

    View view(canvas, workspace, 15);
    viewPtr = &view;
    canvasPtr = &canvas;

    populateVBO(canvas);



    glClearColor(247.0f / 255.0f,
        247.0f / 255.0f,
        242.0f / 255.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    while (!glfwWindowShouldClose(window))
    {
        auto frameStart = std::chrono::steady_clock::now();

        glClear(GL_COLOR_BUFFER_BIT);



        glUseProgram(canvasShader);
        glUniform1f(glGetUniformLocation(canvasShader, "uCellSize"), (float)view.cellSize);
        glUniform2f(glGetUniformLocation(canvasShader, "uOrigin"), view.originX, view.originY);
        glUniform2f(glGetUniformLocation(canvasShader, "uWindowMeasures"), windowWidth, windowHeight);
        glUniform1i(glGetUniformLocation(canvasShader, "uCanvasWidth"), canvas.w);
        glUniform1f(glGetUniformLocation(canvasShader, "uAspectRatio"), uAspectRatio);

        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, canvas.w * canvas.h);
        if (resizing) drawResizePreview(previewShader);
        glfwSwapBuffers(window); 
        glfwPollEvents();


        auto frameEnd = std::chrono::steady_clock::now();
        auto elapsed = frameEnd - frameStart;
        if (elapsed < frameDuration)
            std::this_thread::sleep_for(frameDuration - elapsed);
    }

    glfwTerminate();
    return 0;
}