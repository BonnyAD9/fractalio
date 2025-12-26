#version 460 core

#define max_roots 8

out vec4 frag_color;
in vec2 cor;

uniform sampler1D gradient;

uniform uint flags; // unused

uniform dvec2 center;
uniform double scale;
uniform uint iterations;
uniform float color_count; // unused

uniform uint root_cnt;
uniform vec2[max_roots + 1] coefs;
uniform vec2[max_roots] roots;

#include <ploat>

pvec2 newton_raphson(pvec2 z);
pvec2 cmul(pvec2 a, pvec2 b);
pvec2 cdiv(pvec2 a, pvec2 b);
pvec2 fun(pvec2 z);
pvec2 deriv(pvec2 z);

void main() {
    if (root_cnt == 0) {
        frag_color = vec4(0, 0, 0, 1);
        return;
    }
    
    pvec2 z = pvec2(cor * scale + center);
    
    uint c = 0;
    uint i = iterations;
    ploat md = 0;
    for (; i > 0; --i) {
        z = newton_raphson(z);
        
        pvec2 dirv = pvec2(roots[0]) - z;
        md = dirv.x * dirv.x + dirv.y * dirv.y;
        c = 0;
        for (uint j = 1; j < root_cnt; ++j) {
            dirv = pvec2(roots[j]) - z;
            ploat dist = dirv.x * dirv.x + dirv.y * dirv.y;
            if (dist < md) {
                md = dist;
                c = j;
            }
        }
        if (md < 0.001) {
            break;
        }
    }
    
    vec3 col = vec3(0, 0, 0);
    if (i != 0) {
        float d = 0;
        if ((flags & 0x10u) == 0) {
            d = float(md) * 1000;
        }
        float brightness = 1;
        float cidx = float(c) / root_cnt;
        switch (flags & 0xFu) {
        case 0:
            break;
        case 1:
            brightness = log(1 + (iterations - i + pow(d, 0.128) * 1.7) / color_count);
            break;
        case 2:
            brightness = pow((i - pow(d, 0.128) * 1.7) / iterations, 8);
            break;
        case 3:
            cidx = (iterations - i + pow(d, 0.128) * 1.7) / color_count;
            break;
        case 4:
            cidx = log(1 + (iterations - i + pow(d, 0.128) * 1.7) / color_count);
            break;
        }
        col = texture(gradient, cidx).xyz * brightness;
    }
    
    vec2 pv = (roots[c] - vec2(center)) / float(scale) - cor;
    pv *= pv;
    float pd = pv.x + pv.y;
    if ((flags & 0x20u) == 0 && pd < 0.004 && pd > 0.0015) {
        col = vec3(1, 1, 1) - col;
    }
    
    frag_color = vec4(col, 1);
}

pvec2 newton_raphson(pvec2 z) {
    return z - cdiv(fun(z), deriv(z));
}

pvec2 cmul(pvec2 a, pvec2 b) {
    return pvec2(a.x * b.x - a.y * b.y, a.x * b.y + b.x * a.y);
}

pvec2 cdiv(pvec2 a, pvec2 b) {
    ploat div = b.x * b.x + b.y * b.y;
    return pvec2(
        (a.x * b.x + a.y * b.y) / div,
        (a.y * b.x - a.x * b.y) / div
    );
}

pvec2 fun(pvec2 z) {
    pvec2 res = pvec2(coefs[0]);
    for (uint i = 1; i < root_cnt + 1; ++i) {
        res = cmul(res, z) + pvec2(coefs[i]);
    }
    return res;
}

pvec2 deriv(pvec2 z) {
    pvec2 res = coefs[0] * root_cnt;
    for (uint i = 1; i < root_cnt; ++i) {
        res = cmul(res, z) + (root_cnt - i) * pvec2(coefs[i]);
    }
    return res;
}
