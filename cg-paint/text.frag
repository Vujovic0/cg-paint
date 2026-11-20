#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uGlyph;
uniform vec4 uColor;

void main() {
    float alpha = texture(uGlyph, vUV).r;
    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}