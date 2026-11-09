#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, int button, int action, int mods);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void cursorPosCallback(GLFWwindow* window, double x, double y);
int detectEdges(float mx, float my);
void updateCursor(GLFWwindow* window, int edges);