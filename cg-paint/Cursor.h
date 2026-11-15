#pragma once
#include "cursorShape.h"
#include "Vertex.h"
#include "Colors.h"
#include <vector>

class Cursor {
private:
	std::vector<Vertex> createCircle(float cellSize, double mx, double my);
	std::vector<Vertex> createTriangle(float cellSize, double mx, double my);
	std::vector<Vertex> createSquare(float cellSize, double mx, double my);
public:
	std::vector<Cell> stamp;
	CursorShape shape;
	float size;
	Color color = Colors::BLACK;
	bool mode = true;
	Cursor() : shape(CursorShape::CIRCLE), size(8) {};
	Cursor(CursorShape shape, float size) : shape(shape), size(size) {};
	std::vector<Vertex> generateVertices(float cellSize, double mx, double my);
	void rebuildStamp();
};