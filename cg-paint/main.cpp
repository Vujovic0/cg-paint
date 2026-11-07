#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "shaderUtil.h"

int endProgram(std::string message) {
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {

}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "MiniPaint", NULL, NULL);
    if (window == NULL) return endProgram("Window failed to be created");
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);

    if (glewInit() != GLEW_OK) return endProgram("GLEW failed to initialize");

    unsigned int basicShader = createShader("basic.vert", "basic.frag");

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

    glClearColor(0.5f, 0.6f, 1.0f, 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(basicShader);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(window); 
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}