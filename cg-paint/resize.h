#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "globals.h"
#include "vboHandling.h"
#include "resize.h"

int detectEdges(float mx, float my);
void computeResize(float mx, float my);
void finishResize();
void drawResizePreview(unsigned int shader);