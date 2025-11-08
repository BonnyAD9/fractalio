#version 460 core

#define ploat float
#define pvec2 vec2

out vec4 frag_color;
in vec2 cor;

uniform sampler1D gradient;

uniform dvec2 center;
uniform double scale;
uniform uint iterations;
uniform float color_count;

uniform dvec2 par;

pvec2 to_polar(pvec2 num) {
    return pvec2(
        sqrt(num.x * num.x + num.y * num.y),
        ploat(
            atan( float(num.y), float(num.x) )
        )
    );
}

pvec2 from_polar(pvec2 num) {
    return pvec2(
        num.x * ploat(cos(float(num.y))),
        num.x * ploat(sin(float(num.y)))
    );
}

void main() {
    pvec2 c = pvec2(cor * scale + center);
    pvec2 x = c;
    pvec2 p = pvec2(par);

    uint i = iterations;
    for (; i > 0; --i) {
        x = to_polar(x);

        // |x| > 2
        if (x.x > 16) {
            break;
        }

        x = from_polar(pvec2(
            ploat(pow(float(x.x), float(p.x))) * ploat(exp(float(-p.y * x.y))),
            x.y * p.x + p.y * ploat(log(float(x.x)))
        ));
        x += c;
    }
    
    if (i <= 0) {
        frag_color = vec4(0, 0, 0, 1);
    } else {
        frag_color = texture(gradient, (iterations - i) / color_count);
    }
}