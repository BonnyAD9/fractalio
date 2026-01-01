#version 460 core

out vec4 frag_color;
layout (location = 1) out vec4 state_out;

uniform sampler2D state;
in vec2 cor;

uniform uint flags;
uniform uint action;

uniform dvec2 center;
uniform double scale;
uniform float height;

uniform float step_size;
uniform uint step_cnt;

const float PI = 3.14159265359;

vec4 init();
vec4 to_color(vec4 state);

void main() {
    switch (action) {
    case 0:
        state_out = init();
        break;
    case 1:
        state_out = texture(state, cor / vec2(4, height * 2) + 0.5);
        break;
    default:
        frag_color = to_color(texture(state, cor / vec2(4, height * 2) + 0.5));
        break;
    }
}

vec4 init() {
    const float PO4 = 0.785398163397;
    vec2 pos = vec2(cor * scale + center);
    return vec4(pos * PO4 + PI, 0, 0);
}

vec4 to_color(vec4 s) {
    const float SQRT2 = 1.414213562373095;
    const float PO2 = PI / 2;
    vec2 cps = mod(s.xy, PI) / PO2;
    if (cps.x > 1) {
        cps.x = 2 - cps.x;
    }
    if (cps.y > 1) {
        cps.y = 2 - cps.y;
    }
    return vec4(cps, 1 - length(cps) / SQRT2, 1);
}