#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <chrono>
#include <thread>
#include "shaderUtil.h"
#include "Canvas.h"
#include "CanvasView.h"
#include "globals.h"
#include "resize.h"
#include "edge.h"
#include "callbacks.h"
#include <algorithm>
#include "constants.h"
#include "Minimap.h"
#include "Renderer.h"
#include "Toolbar.h"
#include "drawBrush.h"


float uAspectRatio = 1.0f;

const auto frameDuration = std::chrono::microseconds(1000000 / 75);

int endProgram(std::string message) {
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

GLFWwindow* createFullscreenWindow() {

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    uAspectRatio = (float)mode->width / mode->height;
    gbs::windowWidth = mode->width;
    gbs::windowHeight = mode->height;
    gbs::workspace.height = gbs::windowHeight;
    gbs::workspace.width = gbs::windowWidth;
    gbs::canvasView.fit();

    return glfwCreateWindow(mode->width, mode->height, "MiniPaint", monitor, NULL);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    gbs::curH = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    gbs::curV = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    gbs::curNWSE = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    gbs::curNESW = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);

    GLFWwindow* window = createFullscreenWindow();
    if (window == NULL) return endProgram("Window failed to be created");
    
    glfwMakeContextCurrent(window);
    glfwFocusWindow(window);



    glfwSetScrollCallback(window, scrollCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetCharCallback(window, charCallback);

    if (glewInit() != GLEW_OK) return endProgram("GLEW failed to initialize");

    unsigned int canvasShader = createShader("canvas.vert", "canvas.frag");
    unsigned int basicShader = createShader("basic.vert", "basic.frag");
    unsigned int textShader = createShader("text.vert", "text.frag");

    Renderer renderer("C:/Windows/Fonts/arial.ttf", 18, basicShader, textShader);


    gbs::canvas.at(0, 0) = Color{ 1, 0, 0, 1 };
    gbs::viewPtr = &gbs::canvasView;
    gbs::canvasPtr = &gbs::canvas;
    gbs::cursorPtr = &gbs::cursor;

    Toolbar toolbar(gbs::windowWidth, 40, 0, 0, [](Color c)
        {
            gbs::cursorPtr->color = c;
        });

    gbs::toolbarPtr = &toolbar;

    float maxDim = (float)std::max(gbs::canvas.w, gbs::canvas.h);
    gbs::cursor.radius = std::clamp(maxDim / 200.0f, 0.5f, constants::MAX_CURSOR_SIZE);
    gbs::cursor.rebuildStamp();

    populateVBO(gbs::canvas);

    Minimap minimap = Minimap(2, 2, gbs::windowWidth, gbs::windowHeight, 5);
    gbs::minimapPtr = &minimap;

    glClearColor(247.0f / 255.0f,
        247.0f / 255.0f,
        242.0f / 255.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    double mx, my;

    while (!glfwWindowShouldClose(window))
    {
        auto frameStart = std::chrono::steady_clock::now();

        glfwGetCursorPos(window, &mx, &my);

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(canvasShader);
        glUniform1f(glGetUniformLocation(canvasShader, "uCellSize"), (float)gbs::canvasView.cellSize);
        glUniform2f(glGetUniformLocation(canvasShader, "uOrigin"), gbs::canvasView.originX, gbs::canvasView.originY);
        glUniform2f(glGetUniformLocation(canvasShader, "uWindowMeasures"), gbs::windowWidth, gbs::windowHeight);
        glUniform1i(glGetUniformLocation(canvasShader, "uCanvasCellWidth"), gbs::canvas.w);
        glUniform1f(glGetUniformLocation(canvasShader, "uAspectRatio"), uAspectRatio);

        glBindVertexArray(gbs::canvasVAO);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, gbs::canvas.w * gbs::canvas.h);
        toolbar.draw(renderer);
        minimap.draw(canvasShader);
        if (gbs::viewPtr->isCursorIn(mx, my)) {
            drawBrush(window, basicShader, *gbs::cursorPtr);
        }
        if (gbs::resizing) drawResizePreview(basicShader);
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