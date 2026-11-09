#include "globals.h"

namespace gbs {
	float windowWidth = 0.0f;
	float windowHeight = 0.0f;

	Workspace workspace = Workspace{ 0.0f, 0.0f, 0.0f, 0.0f };
	Canvas canvas = Canvas(320, 180);
	CanvasView canvasView = CanvasView(canvas, workspace, 15);

	CanvasView* viewPtr = nullptr;
	Canvas* canvasPtr = nullptr;

	unsigned int VAO;
	unsigned int quadVBO;
	unsigned int colorVBO;
	unsigned int previewVAO;
	unsigned int previewVBO;

	const float grabPx = 8.0f;
	const int maxCanvasSize = 4096;

	bool resizing = false;
	int resizeEdges = 0;
	int hoverEdges = -1;

	int pendingW = 0;
	int pendingH = 0;
	int pendingOffX = 0;
	int pendingOffY = 0;

	bool panning = false;
	double lastMouseX = 0;
	double lastMouseY = 0;

	GLFWcursor* curH = nullptr;
	GLFWcursor* curV = nullptr;
	GLFWcursor* curNWSE = nullptr;
	GLFWcursor* curNESW = nullptr;
}
