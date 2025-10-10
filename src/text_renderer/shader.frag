#version 460 core

out vec4 frag_color;

in vec2 cor;

uniform sampler2D bitmap;

uniform vec3 color;

void main() {
    vec4 col = texture(bitmap, cor);
    frag_color = vec4(col.rrr * color, 1);
}