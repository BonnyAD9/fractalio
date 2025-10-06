#version 460 core

out vec4 frag_color;
in vec2 cor;

void main() {
    const int LIM = 100;

    vec2 c = cor;
    vec2 x = cor;

    int i = LIM;
    for (; i > 0; --i) {
        float x2 = x.x * x.x;
        float y2 = x.y * x.y;

        // |x| > 2
        if (x2 + y2 > 4) {
            break;
        }

        // (x, y)^2 == (x * x - y * y, 2 * x * y)
        x.y = 2 * x.x * x.y;
        x.x = x2 - y2;

        x += c;
    }

    float res = float(i) / LIM;
    frag_color = vec4(res, res, res, 1);
}
