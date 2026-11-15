#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec4 inColor;
uniform vec2 uWindowMeasures;
out vec4 vColor;

void main()
{
    vec2 ndc = inPos / uWindowMeasures * 2.0 - 1.0;
    ndc.y = -ndc.y;
    gl_Position = vec4(ndc, 0.0, 1.0);
    vColor = inColor;
}