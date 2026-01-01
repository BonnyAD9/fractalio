#version 460 core

out vec4 frag_color;
in vec2 cor;

uniform sampler1D gradient;

uniform uint flags; // unused

uniform dvec2 center;
uniform double scale;
uniform uint iterations;
uniform float color_count;

uniform dvec2 par;

const float PI = 3.14159265359;

#include <ploat>
#include <mb_coloring>

#if PRECISION == 8

#define patan datan
#define pcos dcos
#define psin dsin
#define pexp dexp
#define plog dlog
#define ppow dpow

float after(float v) {
    if (v == -0.) {
        return intBitsToFloat(1);
    }
    if (v < 0) {
        return intBitsToFloat(floatBitsToInt(v) - 1);
    }
    return intBitsToFloat(floatBitsToInt(v) + 1);
}

float before(float v) {
if (v == 0.) {
    return intBitsToFloat(0x80000001);
}
if (v < 0) {
    return intBitsToFloat(floatBitsToInt(v) + 1);
}
return intBitsToFloat(floatBitsToInt(v) - 1);
}

#define PI 3.141592653589793

#define foo_inter(foo, v, res) {\
    float fv = float(v); \
    double dv = double(fv); \
    if (dv < v) { \
        float nv = after(fv); \
        res = mix(double(foo(fv)), double(foo(nv)), (v - dv) / (double(nv) - dv)); \
    } else { \
        float pv = before(fv); \
        double pvd = double(pv); \
        res = mix(double(foo(pv)), double(foo(fv)), (v - pvd) / (dv - pvd)); \
    } \
}

double datan(double y, double x) {
    double v = y / x;
    double res;
    foo_inter(atan, v, res);
    if (x < 0 && y < 0) {
        return res - PI;
    } else if (x < 0 && y > 0) {
        return res + PI;
    }
    return res;
}

double dcos(double n) {
    double res;
    foo_inter(cos, n, res);
    return res;
}

double dsin(double n) {
    double res;
    foo_inter(sin, n, res);
    return res;
}

double dexp(double n) {
    if (n == 0. || n == -0.) {
        return 1;
    }
    double res;
    foo_inter(exp, n, res);
    return res;
}

double dlog(double n) {
    double res;
    foo_inter(log, n, res);
    return res;
}

double dpow(double v, float p) {
    double res;
    float fv = float(v);
    double dv = double(v);
    if (dv < v) {
        float nv = after(fv);
        res = mix(double(pow(fv, p)), double(pow(nv, p)), (v - dv) / (double(nv) - dv));
    } else {
        float pv = before(fv);
        double pvd = double(pv);
        res = mix(double(pow(pv, p)), double(pow(fv, p)), (v - pvd) / (dv - pvd));
    }
    return res;
}

#else

#define patan atan
#define pcos cos
#define psin sin
#define pexp exp
#define plog log
#define ppow pow

#endif

pvec2 to_polar(pvec2 num) {
    return pvec2(
        sqrt(num.x * num.x + num.y * num.y),
        patan(num.y, num.x)
    );
}

pvec2 from_polar(pvec2 num) {
    return pvec2(
        num.x * pcos(num.y),
        num.x * psin(num.y)
    );
}

void main() {
    pvec2 c = pvec2(cor * scale + center);
    pvec2 x = c;
    pvec2 p = pvec2(par.x, par.y);
    
    ploat mz = max(ploat(par.x * par.x + par.y * par.y), 4);

    uint i = iterations;
    for (; i > 0; --i) {
        x = to_polar(x);

        if (x.x > mz) {
            break;
        }

        x = from_polar(pvec2(
            ppow(x.x, float(p.x)) * pexp(-p.y * x.y),
            x.y * p.x + p.y * plog(x.x)
        ));
        x += c;
    }
    
    if (i <= 0) {
        frag_color = vec4(0, 0, 0, 1);
    } else {
        frag_color = mb_coloring(
            gradient,
            flags,
            iterations - i,
            color_count,
            vec2(x)
        );
    }
}