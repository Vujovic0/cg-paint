#include "globals.h"
#include "cursorShape.h"
#include "Toolbar.h"


namespace gbs {
	float windowWidth = 0.0f;
	float windowHeight = 0.0f;

	Workspace workspace = Workspace{ 0.0f, 0.0f, 0.0f, 0.0f };
	Canvas canvas = Canvas(1920, 1200);
	CanvasView canvasView = CanvasView(canvas, workspace, 15);
	Brush cursor = Brush(CursorShape::CIRCLE, 16.0f);

	CanvasView* viewPtr = nullptr;
	Canvas* canvasPtr = nullptr;
	Brush* cursorPtr = nullptr;
	Minimap* minimapPtr = nullptr;
	Toolbar* toolbarPtr = nullptr;


	unsigned int canvasVAO;
	unsigned int quadVBO;
	unsigned int colorVBO;

	unsigned int previewVAO;
	unsigned int previewVBO;

	unsigned int cursorVAO;
	unsigned int cursorVBO;

	unsigned int rectVAO;
	unsigned int rectVBO;

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

	CursorShape cursorShape = CursorShape::CIRCLE;

	bool painting = false;
	int lastPaintCellX = 0;
	int lastPaintCellY = 0;
}
