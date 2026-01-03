#version 460 core

out vec4 frag_color;
layout (location = 1) out vec3 x_out;
layout (location = 2) out vec3 y_out;
layout (location = 3) out vec3 vx_out;
layout (location = 4) out vec3 vy_out;
layout (location = 5) out vec3 ax_out;
layout (location = 6) out vec3 ay_out;

layout (binding = 0) uniform sampler2D x_in;
layout (binding = 1) uniform sampler2D y_in;
layout (binding = 2) uniform sampler2D vx_in;
layout (binding = 3) uniform sampler2D vy_in;
layout (binding = 4) uniform sampler2D ax_in;
layout (binding = 5) uniform sampler2D ay_in;

struct Obj {
    vec2 pos;
    vec2 vel;
    vec2 acc;
};

struct State {
    Obj a;
    Obj b;
    Obj c;
};

in vec2 cor;

uniform uint flags;
uniform uint action;

uniform dvec2 center;
uniform double scale;
uniform float aspect;

uniform float step_size;
uniform uint step_cnt;

uniform vec2 hover;

State get_state();
void set_state(State s);
State init();
vec4 to_color(State s);
State stepn(State s);
State step(State s, float h);
vec2 force(vec2 a, float am, vec2 b, float bm);
vec4 close_col(vec4 def);

const float G = 6.67430e-11;
const float M0 = 100;
const float M1 = 100;
const float M2 = 100;
const float PI = 3.14159265359;

void main() {
    switch (action) {
    case 0:
        set_state(stepn(init()));
        break;
    case 1:
        set_state(stepn(get_state()));
        break;
    default:
        frag_color = close_col(to_color(get_state()));
        break;
    }
}

State get_state() {
    vec2 pos = vec2(cor) / vec2(2, aspect * 2) + 0.5;
    vec3 x = texture(x_in, pos).xyz;
    vec3 y = texture(y_in, pos).xyz;
    vec3 xv = texture(vx_in, pos).xyz;
    vec3 yv = texture(vy_in, pos).xyz;
    vec3 xa = texture(ax_in, pos).xyz;
    vec3 ya = texture(ay_in, pos).xyz;
    return State(
        Obj(vec2(x.x, y.x), vec2(xv.x, yv.x), vec2(xa.x, ya.x)),
        Obj(vec2(x.y, y.y), vec2(xv.y, yv.y), vec2(xa.y, ya.y)),
        Obj(vec2(x.z, y.z), vec2(xv.z, yv.z), vec2(xa.z, ya.z))
    );
}

void set_state(State s) {
    x_out = vec3(s.a.pos.x, s.b.pos.x, s.c.pos.x);
    y_out = vec3(s.a.pos.y, s.b.pos.y, s.c.pos.y);
    vx_out = vec3(s.a.vel.x, s.b.vel.x, s.c.vel.x);
    vy_out = vec3(s.a.vel.y, s.b.vel.y, s.c.vel.y);
    ax_out = vec3(s.a.acc.x, s.b.acc.x, s.c.acc.x);
    ay_out = vec3(s.a.acc.y, s.b.acc.y, s.c.acc.y);
}

State init() {
    State s = State(
        Obj(vec2(cor * scale + center), vec2(0, 0), vec2(0, 0)),
        Obj(vec2(-0.5, 0), vec2(0, 0), vec2(0, 0)),
        Obj(vec2(0.5, 0), vec2(0, 0), vec2(0, 0))
    );
    s.a.vel = normalize(s.b.pos - s.c.pos) * 0.1;
    s.b.vel = normalize(s.c.pos - s.a.pos) * 0.1;
    s.c.vel = normalize(s.a.pos - s.b.pos) * 0.1;
    return s;
}

vec4 to_color(State s) {
    vec3 col = vec3(0, 0, 0);
    bool norm = true;
    switch (flags & 0xFu) {
    default:
        col = vec3(
            length(s.a.pos - s.b.pos),
            length(s.b.pos - s.c.pos),
            length(s.c.pos - s.a.pos)
        );
        break;
    case 1:
        col = vec3(
            length(s.a.pos),
            length(s.b.pos),
            length(s.c.pos)
        );
        break;
    case 2:
        col = vec3(
            length(s.a.vel),
            length(s.b.vel),
            length(s.c.vel)
        );
        break;
    case 3:
        col = vec3(
            abs(atan(s.a.pos.y, s.a.pos.x)) / (PI * 2),
            abs(atan(s.b.pos.y, s.b.pos.x)) / (PI * 2),
            abs(atan(s.c.pos.y, s.c.pos.x)) / (PI * 2)
        );
        norm = false;
        break;
    case 4: {
        vec2 avg = (s.a.pos + s.b.pos + s.c.pos) / 3;
        vec2 aa = s.a.pos - avg;
        vec2 ba = s.b.pos - avg;
        vec2 ca = s.c.pos - avg;
        col = vec3(
            abs(atan(aa.y, aa.x)) / (PI * 2),
            abs(atan(ba.y, ba.x)) / (PI * 2),
            abs(atan(ca.y, ca.x)) / (PI * 2)
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

State stepn(State s) {
    for (uint i = step_cnt; i > 0; --i) {
        s = step(s, step_size);
    }
    return s;
}

State step(State s, float h) {
    vec2 fab = force(s.a.pos, M0, s.b.pos, M1);
    vec2 fbc = force(s.b.pos, M1, s.c.pos, M2);
    vec2 fca = force(s.c.pos, M2, s.a.pos, M0);
    s.a.acc = (fab - fca) / M0;
    s.b.acc = (fbc - fab) / M1;
    s.c.acc = (fca - fbc) / M2;
    s.a.vel += s.a.acc * h;
    s.b.vel += s.b.acc * h;
    s.c.vel += s.c.acc * h;
    s.a.pos += s.a.vel * h;
    s.b.pos += s.b.vel * h;
    s.c.pos += s.c.vel * h;
    return s;
}

vec2 force(vec2 a, float ma, vec2 b, float mb) {
    vec2 diff = b.xy - a.xy;
    float dist = length(diff) * 0.001;
    return normalize(diff) * ma * mb / (dist * dist) * G;
}

bool close(vec2 a, vec2 b, float scl) {
    float pd = length(a - b) / scl;
    return pd < 0.015;
}

vec4 close_col(vec4 def) {
    vec3 xs = texture(x_in, hover).xyz;
    vec3 ys = texture(y_in, hover).xyz;
    
    vec2 a = vec2(xs.x, ys.x);
    vec2 b = vec2(xs.y, ys.y);
    vec2 c = vec2(xs.z, ys.z);
    
    vec2 bl = vec2(
        min(a.x, min(b.x, c.x)),
        min(a.y, min(b.y, c.y))
    );
    vec2 tr = vec2(
        max(a.x, max(b.x, c.x)),
        max(a.y, max(b.y, c.y))
    );
    
    vec2 siz = vec2(tr.x - bl.x, tr.y - bl.y);
    vec2 cen = vec2(bl + siz / 2);
    float scl = float(max(siz.x, siz.y * aspect));
    
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