#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inCol;

out vec4 chCol;

uniform int uCanvasCellWidth;
uniform vec2 uOrigin;
uniform float uCellSize;
uniform vec2 uWindowMeasures;

void main()
{
    vec2 cellVertexCoords = vec2(gl_InstanceID % uCanvasCellWidth, gl_InstanceID / uCanvasCellWidth) + inPos;
    vec2 screenCoords = uOrigin + cellVertexCoords * uCellSize;
    vec2 ndc = screenCoords / uWindowMeasures * 2.0 - 1.0;
    ndc.y = -ndc.y;
    gl_Position = vec4(ndc, 0.0, 1.0);
    chCol = vec4(inCol, 1.0);

}