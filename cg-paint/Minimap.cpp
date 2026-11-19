#include "Minimap.h"
#include "globals.h"
#include <algorithm>

Minimap::Minimap(int marginXPercent, int marginYPercent, int monitorWidth, int monitorHeight, int sizeDivisor)
	: marginXPercent(marginXPercent), marginYPercent(marginYPercent), monitorWidth(monitorWidth), monitorHeight(monitorHeight), sizeDivisor(sizeDivisor) 
{
	reset();
	this->cells = cells;
}

void Minimap::reset() {
	int maxLen = std::min(monitorWidth, monitorHeight) / sizeDivisor;

	if (gbs::canvas.w >= gbs::canvas.h) {
		width = maxLen;
		height = maxLen * gbs::canvas.h / gbs::canvas.w;
	}
	else {
		height = maxLen;
		width = maxLen * gbs::canvas.w / gbs::canvas.h;
	}

	originX = monitorWidth * marginXPercent / 100;
	originY = monitorHeight - monitorHeight * marginYPercent / 100 - height;

	cellSize = std::min(
		(float)width / gbs::canvas.w,
		(float)height / gbs::canvas.h
	);
}

void Minimap::draw(unsigned int shader) {
	glUseProgram(shader);
	glUniform1f(glGetUniformLocation(shader, "uCellSize"), (float)cellSize);
	glUniform2f(glGetUniformLocation(shader, "uOrigin"), originX, originY);
	glUniform2f(glGetUniformLocation(shader, "uWindowMeasures"), gbs::windowWidth, gbs::windowHeight);
	glUniform1i(glGetUniformLocation(shader, "uCanvasCellWidth"), gbs::canvas.w);

	glBindVertexArray(gbs::canvasVAO);
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, gbs::canvas.w * gbs::canvas.h);
}