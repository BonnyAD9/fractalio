#version 460 core

#define ploat float
#define pvec2 vec2

out vec4 frag_color;
in vec2 cor;

uniform sampler1D gradient;

uniform uint flags; // unused

uniform dvec2 center;
uniform double scale;
uniform uint iterations;
uniform float color_count;

void main() {
    pvec2 c = pvec2(cor * scale + center);
    c = pvec2(c.x, -c.y);
    pvec2 x = c;

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
        float brightness = 1;
        float cidx = iterations - i;
        switch (flags & 0xFu) {
        case 0:
            break;
        case 1: {
            cidx += 1 - log(log(float(length(x)))) / 0.6931471805599453;
            break;
        }
        case 2:
            brightness = 1 - log(log(float(length(x)))) / 0.6931471805599453; 
            break;
        }
        
        vec3 col = texture(gradient, cidx / color_count).xyz;
        frag_color = vec4(col * brightness, 1);
    }
}