#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inCol;

out vec4 chCol;

uniform float uAspectRatio;

void main()
{
    if (uAspectRatio >= 1.0)
        gl_Position = vec4(inPos.x / uAspectRatio, inPos.y, 0.0, 1.0);
    else
        gl_Position = vec4(inPos.x, inPos.y * uAspectRatio, 0.0, 1.0);
    chCol = vec4(inCol, 1.0);
}