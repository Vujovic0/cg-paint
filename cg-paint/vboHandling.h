#pragma once
#include "Canvas.h"

void reallocColors();
void populateVBO(Canvas& canvas);
void uploadColors(unsigned int colorVBO, const Canvas& canvas);