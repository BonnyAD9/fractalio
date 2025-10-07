#version 460 core

out vec4 frag_color;
in vec2 cor;

uniform sampler1D gradient;

uniform dvec2 center;
uniform double scale;

void main() {
    const int LIM = 256;
    const int COLOR_COUNT = 256;

    vec2 c = vec2(cor * scale + center);
    lowp vec2 x = c;

    int i = LIM;
    for (; i > 0; --i) {
        vec2 x2 = x * x;

        // |x| > 2
        if (x2.x + x2.y > 4) {
            break;
        }

        // (x, y)^2 == (x * x - y * y, 2 * x * y)
        x.y = 2 * x.x * x.y;
        x.x = x2.x - x2.y;

        x += c;
    }
    
    if (i <= 0) {
        frag_color = vec4(0, 0, 0, 1);
    } else {
        frag_color = texture(gradient, float(LIM - i) / COLOR_COUNT);
    }
}
