#version 460 core

out vec4 frag_color;

#define MODE 0

#if MODE == 1

#define MAX_STORE 8

in vec2 cor;

uniform sampler2D data;

uniform float aspect;
uniform vec2 store[MAX_STORE];
uniform uint store_cnt;
uniform uint flags;

void main() {
    vec4 col = texture(data, cor / vec2(2, aspect * 2) + 0.5);
    if (col.w < 1) {
        col.w = 1;
    } else {
        col /= col.w;
    }
    
    for (uint i = 0; i < store_cnt; ++i) {
        vec2 pv = store[i] - cor;
        pv *= pv;
        float pd = pv.x + pv.y;
        if ((flags & 0x20u) == 0 && pd < 0.002 && pd > 0.00075) {
            col = vec4(1, 1, 1, 2) - col;
            break;
        }
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
