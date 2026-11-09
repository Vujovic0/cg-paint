#pragma once
#include "Canvas.h"
#include "CanvasView.h"
#include "structs.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace gbs {
	extern Canvas canvas;
	extern CanvasView canvasView;
	extern Workspace workspace;
	extern float windowWidth;
	extern float windowHeight;
	extern Canvas* canvasPtr;
	extern CanvasView* viewPtr;
 
	extern unsigned int VAO;
	extern unsigned int quadVBO;
	extern unsigned int colorVBO;
	extern unsigned int previewVAO;
	extern unsigned int previewVBO;

	extern const float grabPx;
	extern const int maxCanvasSize;

	extern bool resizing;
	extern int resizeEdges;
	extern int hoverEdges;

	extern int pendingW;
	extern int pendingH;
	extern int pendingOffX;
	extern int pendingOffY;

	extern bool panning;
	extern double lastMouseX;
	extern double lastMouseY;

	extern GLFWcursor* curH;
	extern GLFWcursor* curV;
	extern GLFWcursor* curNWSE;
	extern GLFWcursor* curNESW;
}