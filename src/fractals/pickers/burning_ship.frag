#version 460 core

out vec4 frag_color;
in vec2 cor;

uniform sampler1D gradient;

uniform uint flags; // unused

uniform dvec2 center;
uniform double scale;
uniform uint iterations;
uniform float color_count;

uniform vec2 par;

#include <ploat>

void main() {
    pvec2 c = pvec2(cor * scale + center);
    c = pvec2(c.x, -c.y);
    pvec2 x = abs(c);

    uint i = iterations;
    for (; i > 0; --i) {
        pvec2 x2 = x * x;

        // |x| > 2
        if (x2.x + x2.y > 4) {
            break;
        }

        // (x, y)^2 == (x * x - y * y, 2 * x * y)
        x.y = 2 * x.x * x.y;
        x.x = x2.x - x2.y;

        x = abs(x + c);
    }
    
    if (i <= 0) {
        frag_color = vec4(0, 0, 0, 1);
    } else {
        frag_color = texture(gradient, (iterations - i) / color_count);
    }
    
    vec2 pv = par - cor;
    pv *= pv;
    float pd = pv.x + pv.y;
    if (pd < 0.004 && pd > 0.002) {
        frag_color = vec4(vec3(1, 1, 1) - frag_color.xyz, 1);
    }
}