#version 330 core
layout (location = 0) in vec4 aVertex;
out vec2 vUV;
uniform vec2 uWindowMeasures;

void main() {
    vec2 ndc = (aVertex.xy / uWindowMeasures) * 2.0 - 1.0;
    ndc.y = -ndc.y;
    gl_Position = vec4(ndc, 0.0, 1.0);
    vUV = aVertex.zw;
}