#pragma once
#include "cursorShape.h"
#include "Vertex.h"
#include "Colors.h"
#include <vector>

class Brush {
private:
	std::vector<Vertex> createCircle(float cellSize, double mx, double my);
	std::vector<Vertex> createTriangle(float cellSize, double mx, double my);
	std::vector<Vertex> createSquare(float cellSize, double mx, double my);
public:
	std::vector<Cell> stamp;
	CursorShape shape;
	float radius;
	Color color = Colors::BLACK;
	bool isBrush = true;
	Brush() : shape(CursorShape::CIRCLE), radius(8) {};
	Brush(CursorShape shape, float size) : shape(shape), radius(size) {};
	std::vector<Vertex> generateVertices(float cellSize, double mx, double my);
	void rebuildStamp();
};