#version 460 core

#define max_bodies 8

out vec4 frag_color;
in vec2 cor;

uniform sampler1D gradient;

uniform uint flags;

uniform dvec2 center;
uniform double scale;

uniform float time;
uniform float step_size;

uniform uint body_cnt;
uniform vec2[max_bodies] bodies;

// uniform vec2 hover;

vec4 step(vec4 s);
vec4 step_euler(vec4 s, float h);
vec4 step_rk4(vec4 s, float h);
vec2 force(vec2 a, float am, vec2 b, float bm);
vec2 get_acceleration(vec4 s);
vec4 derive(vec4 s, float h);
// bool close(vec2 a, vec2 b, float scl);
//vec4 close_col(vec4 def);

const float G = 0.001; //6.67430e-11;
const float MB = 1; //1000;
const float M = 1; //10;
const float PI = 3.14159265359;
const float DRAG = 0.01;
const float SCALE = 0.02;
const float PSIZE = 0;
const float STABILIZATION = 0.0001;

void main() {
    vec4 s = vec4(cor * scale + center, 0, 0);
    float t = 0;
    uint close = 0;
    
    for (; t < time; t += step_size) {
        float bdist = 9999999;
        for (uint i = 0; i < body_cnt; ++i) {
            vec2 bd = bodies[i] - s.xy;
            float dist = bd.x * bd.x + bd.y * bd.y;
            if (dist <= PSIZE * PSIZE) {
                bdist = dist;
                close = i;
                break;
            }
            if (dist < bdist) {
                bdist = dist;
                close = i;
            }
        }
        if (bdist <= PSIZE * PSIZE) {
            break;
        }
        s = step(s);
    }
    
    vec3 col = texture(gradient, float(close) / body_cnt).xyz;
    vec2 pv = (bodies[close] - vec2(center)) / float(scale) - cor;
    pv *= pv;
    float pd = pv.x + pv.y;
    if ((flags & 0x200u) == 0 && pd < 0.004 && pd > 0.0015) {
        col = vec3(1, 1, 1) - col;
    }
    
    frag_color = vec4(col, 1);
    //frag_color = vec4(cor + bodies[0], 0, 1);
}

vec4 step(vec4 s) {
    switch ((flags >> 4) & 0xFu) {
    default:
        s = step_euler(s, step_size);
        break;
    case 1:
        s = step_rk4(s, step_size);
        break;
    }
    s.zw *= pow(1 - DRAG, step_size);
    return s;
}

vec2 get_acceleration(vec4 s) {
    vec2 f = vec2(0, 0);
    for (uint i = 0; i < body_cnt; ++i) {
        f += force(s.xy, M, bodies[i], MB);
    }
    return f / M;
}

vec4 derive(vec4 s, float h) {
    vec2 acc = get_acceleration(s);
    s.zw += acc * h;
    return vec4(s.zw, acc);
}

vec4 step_euler(vec4 s, float h) {
    return s + derive(s, h) * h;
}

vec4 step_rk4(vec4 x, float h) {
    float ho2 = h / 2;
    
    vec4 k1 = derive(x, 0);
    vec4 k2 = derive(x + k1 * ho2, ho2);
    vec4 k3 = derive(x + k2 * ho2, ho2);
    vec4 k4 = derive(x + h * k3, h);
    
    return x + h * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
}

vec2 force(vec2 a, float ma, vec2 b, float mb) {
    vec2 diff = b.xy - a.xy;
    float dist = length(diff) * SCALE;
    return normalize(diff) * ma * mb / (dist * dist + STABILIZATION) * G;
}