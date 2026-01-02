#version 460 core

out vec4 frag_color;
layout (location = 1) out vec4 state_out;

uniform sampler2D state;

in vec2 cor;

uniform uint flags;
uniform uint action;

uniform dvec2 center;
uniform double scale;
uniform float aspect;

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
vec4 step_rk1(vec4 s);
vec2 get_acceleration(vec4 s);
vec4 derive(vec4 s, float t);
vec4 stepn(vec4 s);
vec4 stepn_rk1(vec4 s);
vec4 stepn_rk4(vec4 s);

void main() {
    switch (action) {
    case 0:
        state_out = stepn_rk4(init());
        break;
    case 1:
        state_out = stepn_rk4(get_state());
        break;
    default:
        frag_color = to_color(get_state());
        break;
    }
}

vec4 get_state() {
    vec2 pos = vec2(cor) / vec2(2, aspect * 2) + 0.5;
    return texture(state, pos);
}

vec4 init() {
    vec2 pos = vec2(cor * scale + center);
    if ((flags & 0x100u) == 0) {
        return vec4(pos * PI, 0, 0);
    } else {
        return vec4(0, 0, pos);
    }
}

vec4 to_color(vec4 s) {
    const float SQRT2 = 1.414213562373095;
    vec2 cps = mod(s.xy, PI) / PO2;
    switch (flags & 0xFu) {
    case 1:
        cps = s.zw;
        break;
    case 2:
        cps = (get_acceleration(s) + 4) / 8;
        break;
    }
    if (cps.x > 1) {
        cps.x = 2 - cps.x;
    }
    if (cps.y > 1) {
        cps.y = 2 - cps.y;
    }
    return vec4(cps, 1 - length(cps) / SQRT2, 1);
}

vec4 step_rk1(vec4 x, float h) {
    return x + derive(x, h) * h;
}

vec4 step_rk4(vec4 x, float h) {
    float ho2 = h / 2;
    
    vec4 k1 = derive(x, 0);
    vec4 k2 = derive(x + k1 * ho2, ho2);
    vec4 k3 = derive(x + k2 * ho2, ho2);
    vec4 k4 = derive(x + h * k3, h);
    
    return x + h * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
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

vec4 derive(vec4 s, float t) {
    vec2 acc = get_acceleration(s);
    s.zw += acc * t;
    return vec4(s.zw, acc);
}

vec4 stepn(vec4 s) {
    switch ((flags >> 4) & 0xFu) {
    case 1:
        return stepn_rk4(s);
    default:
        return stepn_rk1(s);
    }
}

vec4 stepn_rk1(vec4 state) {
    for (uint n = step_cnt; n > 0; --n) {
        state = step_rk1(state, step_size);
    }
    return state;
}

vec4 stepn_rk4(vec4 state) {
    for (uint n = step_cnt; n > 0; --n) {
        state = step_rk4(state, step_size);
    }
    return state;
}