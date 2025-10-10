#version 460 core

out vec4 frag_color;

in highp vec2 cor;

uniform sampler2D bitmap;

uniform vec3 color;

void main() {
    vec4 col = texture(bitmap, cor);
    frag_color = vec4(color, col.r);
}