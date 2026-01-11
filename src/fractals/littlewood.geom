#version 460 core

#define MAX_DEGREE 32
#define MAX_STORE 8

layout (points) in;
layout (points, max_vertices = MAX_DEGREE + 1) out;

in uint idx[];
out flat float cor;

uniform double scale;
uniform dvec2 center;
uniform float aspect;

uniform uint degree;

uniform uint store_cnt;
uniform vec2 store[MAX_STORE];

vec2 coefs[MAX_DEGREE + 1];
vec2 roots[MAX_DEGREE];

void durnad_kerner2(float eps, uint lim);
float durnad_kerner();
void abeth2(float eps, uint lim);
float abeth();
void init_roots(float l);
void init_roots2();
float get_lim();
void to_monic();
float sq_len(vec2 v);
void fill_coefs();
vec2 cmul(vec2 a, vec2 b);
vec2 cdiv(vec2 a, vec2 b);
vec2 fun(vec2 z);
vec2 deriv(vec2 z);

const float EPS = 0.001 * 0.001;
const uint MAX_ITERATIONS = 256;

void main() {
    fill_coefs();
    to_monic();
    init_roots2();
    //init_roots(get_lim());
    abeth2(EPS, MAX_ITERATIONS);
    //durnad_kerner2(EPS, MAX_ITERATIONS);
    
    cor = float(double(idx[0]) / double(uint(1) << degree));
    for (int i = 0; i < degree; ++i) {
        /*float rp = length(roots[i]);
        if (rp < 1.3 && rp > 0.8) {
            continue;
        }*/
        gl_Position = vec4((roots[i] - center) / (dvec2(1, aspect) * scale), 0, 1);
        EmitVertex();
    }
    EndPrimitive();
}

void durnad_kerner2(float eps, uint lim) {
    for (; lim > 0; --lim) {
        if (durnad_kerner() < eps) {
            break;
        }
    }
}

float durnad_kerner() {
    float worst = 0;
    for (int i = 0; i < degree; ++i) {
        vec2 root = roots[i];
        vec2 difs = vec2(1, 0);
        for (int j = 0; j < degree; ++j) {
            if (i != j) {
                difs = cmul(difs, root - roots[j]);
            }
        }
        vec2 w = cdiv(fun(root), difs);
        float wlen = sq_len(w);
        if (wlen > worst) {
            worst = wlen;
        }
        roots[i] -= w;
    }
    return worst;
}

void abeth2(float eps, uint lim) {
    for (; lim > 0; --lim) {
        if (abeth() < eps) {
            break;
        }
    }
}

float abeth() {
    float worst = 0;
    for (int i = 0; i < degree; ++i) {
        vec2 root = roots[i];
        vec2 newt = cdiv(fun(root), deriv(root));
        vec2 repuls = vec2(0, 0);
        for (int j = 0; j < degree; ++j) {
            if (j != i) {
                repuls += cdiv(vec2(1, 0), root - roots[j]);
            }
        }
        vec2 w = cdiv(newt, vec2(1, 0) - cmul(newt, repuls));
        float wlen = sq_len(w);
        if (wlen > worst) {
            worst = wlen;
        }
        roots[i] -= w;
    }
    return worst;
}

void init_roots2() {
    vec2 n = vec2(0.4, 0.9);
    roots[0] = vec2(1, 0);
    roots[1] = n;
    int off = int(degree) / 2;
    for (int i = 2; i < degree; ++i) {
        roots[i] = cmul(roots[i - 1], n);
    }
}

void init_roots(float l) {
    int off = int(degree) / 2;
    for (int i = 0; i < degree; ++i) {
        roots[i] = vec2(float(i - off) / float(off) * l, 1);
    }
}

float get_lim() {
    //coefs[0] = coefs[0]; // suppress uninitialized warning (it is initialized)
    float dv = sq_len(coefs[0]);
    float mx = dv;
    for (uint i = 1; i < degree + 1; ++i) {
        float v = sq_len(coefs[i]);
        if (v > mx) {
            mx = v;
        }
    }
    return sqrt(mx / dv);
}

void to_monic() {
    vec2 d = coefs[0];
    for (int i = 0; i < degree + 1; ++i) {
        coefs[i] = cdiv(coefs[i], d);
    }
}

float sq_len(vec2 v) {
    return v.x * v.x + v.y * v.y;
}

void fill_coefs() {
    uint ix = idx[0];
    coefs[degree] = vec2(1, 0);
    for (uint i = 0; i <= degree; ++i) {
        coefs[i] = store[ix % store_cnt];
        ix /= store_cnt;
    }
    /*for (int i = int(degree) - 1; i >= 0; --i) {
        coefs[i] = store[ix % store_cnt];
        ix /= store_cnt;
    }*/
}

vec2 cmul(vec2 a, vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + b.x * a.y);
}

vec2 cdiv(vec2 a, vec2 b) {
    float div = b.x * b.x + b.y * b.y;
    return vec2(
        (a.x * b.x + a.y * b.y) / div,
        (a.y * b.x - a.x * b.y) / div
    );
}

vec2 fun(vec2 z) {
    vec2 res = vec2(coefs[0]);
    for (uint i = 1; i < degree + 1; ++i) {
        res = cmul(res, z) + vec2(coefs[i]);
    }
    return res;
}

vec2 deriv(vec2 z) {
    vec2 res = coefs[0] * degree;
    for (uint i = 1; i < degree; ++i) {
        res = cmul(res, z) + (degree - i) * vec2(coefs[i]);
    }
    return res;
}