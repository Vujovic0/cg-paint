#pragma once
#include "Canvas.h"
#include "CanvasView.h"
#include "structs.h"
#include "Cursor.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace gbs {
	extern Canvas canvas;
	extern CanvasView canvasView;
	extern Workspace workspace;
	extern Cursor cursor;
	extern float windowWidth;
	extern float windowHeight;
	extern Canvas* canvasPtr;
	extern CanvasView* viewPtr;
	extern Cursor* cursorPtr;
 
	extern unsigned int canvasVAO;
	extern unsigned int quadVBO;
	extern unsigned int colorVBO;
	extern unsigned int previewVAO;
	extern unsigned int previewVBO;
	extern unsigned int cursorVAO;
	extern unsigned int cursorVBO;

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

	extern CursorShape cursorShape;
}