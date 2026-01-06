#version 460 core

out vec4 frag_color;

#define MODE 0

#if MODE == 1

in vec2 cor;

uniform sampler2D data;

uniform float aspect;

void main() {
    vec4 col = texture(data, cor / vec2(2, aspect * 2) + 0.5);
    if (col.w < 1) {
        col.w = 1;
    } else {
        col /= col.w;
    }
    frag_color = col;
}

#else

in float cor;

uniform sampler1D gradient;

void main() {
    frag_color = texture(gradient, cor);
    //frag_color = vec4(1, 1, 1, 1); //texture(gradient, cor);
}

#endif
