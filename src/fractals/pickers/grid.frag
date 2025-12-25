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

uniform vec2 par;

float dist_to_scale(float n, float sl) {
    return abs(n - round(n / sl) * sl) / float(scale);
}

float dist_to_scale(vec2 n, float sl) {
    return min(dist_to_scale(n.x, sl), dist_to_scale(n.y, sl));
}

float aa_dist_to_scale(vec2 n, float sl, float th, float aa) {
    float dist = dist_to_scale(n, sl);
    if (dist < th) {
        return 1;
    }
    if (dist < th + aa) {
        return 1 - (dist - th) / aa;
    }
    return 0;
}

vec2 closeness_to_scale(float scale, vec2 n) {
    float scale1 = pow(10, round(log2(float(scale)) / 3.321928));
    float scale0 = scale1 * 10;
    float scale2 = scale1 / 10;
    
    float aa = 0.01;
    
    float dist = aa_dist_to_scale(n, scale0, 0.015, aa);
    if (dist > 0) {
        return vec2(1, dist);
    }
    
    dist = aa_dist_to_scale(n, scale1, 0.01, aa);
    if (dist > 0) {
        return vec2(2, dist);
    }
    
    dist = aa_dist_to_scale(n, scale2, 0.005, aa);
    if (dist > 0) {
        return vec2(3, dist);
    }
    
    return vec2(0, 1);
}

void main() {
    vec2 n = vec2(cor * scale + center);
    
    vec2 i = closeness_to_scale(float(scale), n);
    
    frag_color = vec4(texture(gradient, i.x / 4).xyz * i.y + texture(gradient, 0.f).xyz * (1 - i.y), 1);
    
    vec2 pv = par - cor;
    pv *= pv;
    float pd = pv.x + pv.y;
    if (pd < 0.004 && pd > 0.002) {
        frag_color = vec4(vec3(1, 1, 1) - frag_color.xyz, 1);
    }
}