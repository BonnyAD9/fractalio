#ifndef MB_COLORING_INCLUDED
#define MB_COLORING_INCLUDED

#include <ploat>

float lla(pvec2 x) {
    const float ILN2 = 1.442695040888963;
    return log(log(float(length(x)))) * ILN2;
}

vec4 mb_coloring(sampler1D gradient, uint flags, float i, float cc, vec2 x) {
    float brightness = 1;
    switch (flags & 0xFu) {
    case 1:
        i = (i + 1 - lla(x)) / cc;
        break;
    case 2:
        brightness = 1 - lla(x);
        i /= cc;
        break;
    case 3:
        i = log(1 + (i - lla(x)) / cc);
        break;
    case 4:
        i = log(1 + (i + lla(x)) / cc);
        break;
    case 5: {
        float llax = lla(x);
        brightness = 1 - llax;
        i = log(1 + (i - llax) / cc);
        break;
    }
    default:
        i /= cc;
        break;
    }
    
    return vec4(texture(gradient, i).xyz * brightness, 1);
}

#endif // MB_COLORING_INCLUDED
