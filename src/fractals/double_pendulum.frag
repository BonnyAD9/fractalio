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
const float PO2 = PI / 2;
// Gravity
const float G = 9.8;
// Masses
const float M1 = 1;
const float M2 = 1;
// Lengths
const float L1 = 1;
const float L2 = 1;

vec4 get_state();
vec4 init();
vec4 to_color(vec4 s);
vec4 step(vec4 s);
vec2 get_acceleration(vec4 s);
vec4 stepn(vec4 s, uint n);

void main() {
    switch (action) {
    case 0:
        state_out = stepn(init(), step_cnt);
        break;
    case 1:
        state_out = stepn(texture(state, cor / vec2(4, height * 2) + 0.5), step_cnt);
        break;
    default:
        frag_color = to_color(texture(state, cor / vec2(4, height * 2) + 0.5));
        break;
    }
}

vec4 get_state() {
    vec2 pos = vec2(cor) / vec2(4, height * 2) + 0.5;
    return texture(state, pos);
}

vec4 init() {
    vec2 pos = vec2(cor * scale + center);
    return vec4(pos * PO2, 0, 0);
}

vec4 to_color(vec4 s) {
    const float SQRT2 = 1.414213562373095;
    vec2 cps = mod(s.xy, PI) / PO2;
    if (cps.x > 1) {
        cps.x = 2 - cps.x;
    }
    if (cps.y > 1) {
        cps.y = 2 - cps.y;
    }
    return vec4(cps, 1 - length(cps) / SQRT2, 1);
}

vec4 step(vec4 s) {
    s.zw += get_acceleration(s) * step_size;
    s.xy += s.zw * step_size;
    return s;
}

vec2 get_acceleration(vec4 s) {
    // better naming
    float o1 = s.x;
    float o2 = s.y;
    float w1 = s.z;
    float w2 = s.w;
    
    // precalculate common terms
    float od = o1 - o2;
    float sod = sin(od);
    float cod = cos(od);
    float l1w2 = L1 * w1 * w1;
    float l2w2 = L2 * w2 * w2;
    float m = M1 + M2;
    float d = 2 * M1 + M2 * (1 - cos(2 * od));
    
    // Calculate acceleration of pendulum 1
    float dw1 = -G * ((2 * M1 + M2) * sin(o1) + M2 * sin(o1 - 2 * o2))
        - 2 * sod * M2 * (l2w2 + l1w2 * cod);
    dw1 /= L1 * d;
    
    // calculate acceleration of penulum 2
    float dw2 = 2 * sod * (m * (l1w2 + cos(o1) * G) + l2w2 * M2 * cod);
    dw2 /= L2 * d;
    
    return vec2(dw1, dw2);
}

vec4 stepn(vec4 state, uint n) {
    for (; n > 0; --n) {
        state = step(state);
    }
    return state;
}