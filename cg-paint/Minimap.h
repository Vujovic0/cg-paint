#pragma once
#include <vector>
#include "structs.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Minimap {
public:
	Minimap(int marginXPercent, int marginYPercent, int monitorWidth, int monitorHeight, int sizeDivisor);
	int marginXPercent;
	int marginYPercent;
	int monitorWidth;
	int monitorHeight;
	int sizeDivisor;
	int originX;
	int originY;
	int width;
	int height;
	std::vector<Color> *cells;
	float cellSize;

	void reset();
	void draw(unsigned int shader);
};