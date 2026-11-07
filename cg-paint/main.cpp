#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>
#include <thread>
#include "shaderUtil.h"


float uAspectRatio = 1.0f;
const auto frameDuration = std::chrono::microseconds(1000000 / 75);

int endProgram(std::string message) {
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        std::cout << "User pressed escape" << std::endl;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
}

GLFWwindow* createFullscreenWindow() {

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    uAspectRatio = (float)mode->width / mode->height;

    return glfwCreateWindow(mode->width, mode->height, "MiniPaint", monitor, NULL);
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

    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);

    if (glewInit() != GLEW_OK) return endProgram("GLEW failed to initialize");

    unsigned int canvasShader = createShader("canvas.vert", "canvas.frag");

    float vertices[] = {
        //   x      y       r     g     b        s     t
           -0.2f,  0.2f,   0.0f, 0.0f, 1.0f,    0.0f, 1.0f, // gornje levo teme
           -0.2f, -0.2f,   0.0f, 1.0f, 0.0f,    0.0f, 0.0f, // donje levo teme
            0.2f, -0.2f,   1.0f, 0.0f, 0.0f,    1.0f, 0.0f, // donje desno teme
            0.2f,  0.2f,   0.0f, 1.0f, 1.0f,    1.0f, 1.0f, // gornje desno teme
    };

    unsigned int VAO;
    unsigned int VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

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

        glUniform1f(glGetUniformLocation(canvasShader, "uAspectRatio"), uAspectRatio);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

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