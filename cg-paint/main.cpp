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

const auto frameDuration = std::chrono::microseconds(1000000 / 75);

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
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (!viewPtr) return;
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    viewPtr->zoomAt((float)mx, (float)my, powf(1.2f, (float)yoffset));
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    if (panning && viewPtr) {
        viewPtr->pan((float)(x - lastMouseX), (float)(y - lastMouseY));
        lastMouseX = x;
        lastMouseY = y;
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

void populateVBO(unsigned int &VAO, unsigned int &quadVBO, unsigned int &colorVBO, Canvas &canvas) {
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
}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


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
   
    Canvas canvas(3, 2);
    canvas.at(0, 0) = Color{ 1, 0, 0, 1 };

    Workspace workspace = Workspace{ 0.0f, 0.0f, windowWidth, windowHeight };

    View view(canvas, workspace, 15);
    viewPtr = &view;
    canvasPtr = &canvas;

    populateVBO(VAO, quadVBO, colorVBO, canvas);

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