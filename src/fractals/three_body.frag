#version 460 core

out vec4 frag_color;
layout (location = 1) out vec4 out0;
layout (location = 2) out vec4 out1;
layout (location = 3) out vec4 out2;

layout (binding = 0) uniform sampler2D in0;
layout (binding = 1) uniform sampler2D in1;
layout (binding = 2) uniform sampler2D in2;

in vec2 cor;

uniform uint flags;
uniform uint action;

uniform dvec2 center;
uniform double scale;
uniform float aspect;

uniform float step_size;
uniform uint step_cnt;

uniform vec2 hover;

mat3x4 get_state();
void set_state(mat3x4 s);
mat3x4 init();
float acol(vec2 dir);
vec4 to_color(mat3x4 s);
mat3x4 stepn(mat3x4 s);
mat3x4 stepn_euler(mat3x4 s);
mat3x4 step_euler(mat3x4 s, float h);
mat3x4 stepn_rk4(mat3x4 s);
mat3x4 step_rk4(mat3x4 s, float h);
vec2 force(vec2 a, float am, vec2 b, float bm);
bool close(vec2 a, vec2 b, float scl);
vec4 close_col(vec4 def);
mat3x2 get_acceleration(mat3x4 s);
mat3x4 derive(mat3x4 s, float h);

uniform float g = 6.67430e-11;
uniform float m0 = 100;
uniform float m1 = 100;
uniform float m2 = 100;
uniform float sscale = 0.001;
uniform float stabilization = 0.00000001;
uniform vec2 init1 = vec2(-0.5, 0);
uniform vec2 init2 = vec2(0.5, 0);

const float PI = 3.14159265359;

void main() {
    switch (action) {
    case 0:
        set_state(stepn(init()));
        return;
    case 1:
        set_state(stepn(get_state()));
        return;
    default:
        break;
    }
    
    vec4 col = close_col(to_color(get_state()));
    
    if ((flags & 0x200u) == 0) {
        vec2 pv = (init1 - vec2(center)) / float(scale) - cor;
        pv *= pv;
        float d1 = pv.x + pv.y;
        
        pv = (init2 - vec2(center)) / float(scale) - cor;
        pv *= pv;
        float d2 = pv.x + pv.y;
        
        if ((d1 < 0.002 && d1 > 0.00075) || (d2 < 0.002 && d2 > 0.00075)) {
            col = vec4(1, 1, 1, 2) - col;
        }
    }
    
    frag_color = col;
}

mat3x4 get_state() {
    vec2 pos = vec2(cor) / vec2(2, aspect * 2) + 0.5;
    return mat3x4(
        texture(in0, pos),
        texture(in1, pos),
        texture(in2, pos)
    );
}

void set_state(mat3x4 s) {
    out0 = s[0];
    out1 = s[1];
    out2 = s[2];
}

mat3x4 init() {
    mat3x4 s = mat3x4(
        vec4(cor * scale + center, 0, 0),
        vec4(init1, 0, 0),
        vec4(init2, 0, 0)
    );
    s[0].zw = normalize(s[1].xy - s[2].xy) * 0.1;
    s[1].zw = normalize(s[2].xy - s[0].xy) * 0.1;
    s[2].zw = normalize(s[0].xy - s[1].xy) * 0.1;
    return s;
}

float acol(vec2 dir) {
    return abs(atan(dir.y, dir.x)) / (PI * 2);
}

vec4 to_color(mat3x4 s) {
    vec3 col = vec3(0, 0, 0);
    bool norm = true;
    switch (flags & 0xFu) {
    default:
        col = vec3(
            length(s[0].xy - s[1].xy),
            length(s[1].xy - s[2].xy),
            length(s[2].xy - s[0].xy)
        );
        break;
    case 1:
        col = vec3(
            length(s[0].xy),
            length(s[1].xy),
            length(s[2].xy)
        );
        break;
    case 2:
        col = vec3(
            length(s[0].zw),
            length(s[1].zw),
            length(s[2].zw)
        );
        break;
    case 3:
        col = vec3(
            abs(atan(s[0].y, s[0].x)) / (PI * 2),
            abs(atan(s[1].y, s[1].x)) / (PI * 2),
            abs(atan(s[2].y, s[2].x)) / (PI * 2)
        );
        norm = false;
        break;
    case 4: {
        vec2 avg = (s[0].xy + s[1].xy + s[2].xy) / 3;
        vec2 aa = s[0].xy - avg;
        vec2 ba = s[1].xy - avg;
        vec2 ca = s[2].xy - avg;
        col = vec3(
            abs(atan(aa.y, aa.x)) / (PI * 2),
            abs(atan(ba.y, ba.x)) / (PI * 2),
            abs(atan(ca.y, ca.x)) / (PI * 2)
        );
        norm = false;
        break;
    }
    case 5: {
        vec2 d01 = s[1].xy - s[0].xy;
        vec2 d12 = s[2].xy - s[1].xy;
        vec2 d20 = s[0].xy - s[2].xy;
        float l01 = length(d01);
        float l12 = length(d12);
        float l20 = length(d20);
        col = vec3(
            acol(l01 < l20 ? d01 : -d20),
            acol(l12 < l01 ? d12 : -d01),
            acol(l20 < l12 ? d20 : -d12)
        );
        norm = false;
        break;
    }
    }
    if (norm) {
        col /= max(col.r, max(col.g, col.b));
    }
    return vec4(col, 1);
}

mat3x4 stepn_euler(mat3x4 s) {
    for (uint i = step_cnt; i > 0; --i) {
        s = step_euler(s, step_size);
    }
    return s;
}

mat3x4 stepn_rk4(mat3x4 s) {
    for (uint i = step_cnt; i > 0; --i) {
        s = step_rk4(s, step_size);
    }
    return s;
}

mat3x4 stepn(mat3x4 s) {
    switch ((flags >> 4) & 0xFu) {
    default:
        return stepn_euler(s);
    case 1:
        return stepn_rk4(s);
    }
}

mat3x2 get_acceleration(mat3x4 s) {
    vec2 fab = force(s[0].xy, m0, s[1].xy, m1);
    vec2 fbc = force(s[1].xy, m1, s[2].xy, m2);
    vec2 fca = force(s[2].xy, m2, s[0].xy, m0);
    return mat3x2(
        (fab - fca) / m0,
        (fbc - fab) / m1,
        (fca - fbc) / m2
    );
}

mat3x4 derive(mat3x4 s, float h) {
    mat3x2 acc = get_acceleration(s);
    s[0].zw += acc[0] * h;
    s[1].zw += acc[1] * h;
    s[2].zw += acc[2] * h;
    return mat3x4(
        vec4(s[0].zw, acc[0]),
        vec4(s[1].zw, acc[1]),
        vec4(s[2].zw, acc[2])
    );
}

mat3x4 step_euler(mat3x4 s, float h) {
    return s + derive(s, h) * h;
}

mat3x4 step_rk4(mat3x4 x, float h) {
    float ho2 = h / 2;
    
    mat3x4 k1 = derive(x, 0);
    mat3x4 k2 = derive(x + k1 * ho2, ho2);
    mat3x4 k3 = derive(x + k2 * ho2, ho2);
    mat3x4 k4 = derive(x + h * k3, h);
    
    return x + h * (k1 + 2 * k2 + 2 * k3 + k4) / 6;
}

vec2 force(vec2 a, float ma, vec2 b, float mb) {
    vec2 diff = b.xy - a.xy;
    float dist = length(diff) * sscale;
    return normalize(diff) * ma * mb / (dist * dist + stabilization) * g;
}

bool close(vec2 a, vec2 b, float scl) {
    float pd = length(a - b) / scl;
    return pd < 0.015;
}

vec4 close_col(vec4 def) {
    vec2 a = texture(in0, hover).xy;
    vec2 b = texture(in1, hover).xy;
    vec2 c = texture(in2, hover).xy;
    
    vec2 bl = vec2(
        min(a.x, min(b.x, c.x)),
        min(a.y, min(b.y, c.y))
    );
    vec2 tr = vec2(
        max(a.x, max(b.x, c.x)),
        max(a.y, max(b.y, c.y))
    );
    
    vec2 cen = vec2(center);
    float scl = float(scale);
    if ((flags & 0x100u) == 0) {
        vec2 siz = vec2(tr.x - bl.x, tr.y - bl.y);
        cen = vec2(bl + siz / 2);
        scl = float(max(siz.x, siz.y / aspect));
    }
    
    vec2 t = vec2(cor * scl + cen);
    
    float brightness = log((1 + 1 / scl) * 1.5);
    if (close(t, a, scl)) {
        return vec4(brightness, 0, 0, 1);
    }
    if (close(t, b, scl)) {
        return vec4(0, brightness, 0, 1);
    }
    if (close(t, c, scl)) {
        return vec4(0, 0, brightness, 1);
    }
    return def;
}