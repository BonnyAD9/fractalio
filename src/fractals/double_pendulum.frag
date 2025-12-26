#version 460 core

out vec4 frag_color;

uniform sampler2D state;
in vec2 cor;

uniform uint flags;
uniform uint action;

uniform dvec2 center;
uniform double scale;

uniform float step_size;
uniform uint step_cnt;

vec4 step(vec4 state) {
    return vec4(state.x + 0.1 * step_size, state.yzw);
}

vec4 stepn(vec4 state) {
    for (uint i = step_cnt; i > 0; --i) {
        state = step(state);
    }
    return state;
}

void main() {
    switch (action) {
    case 0: // init
        frag_color = stepn(vec4(0, 1, 0, 1));
        break;
    case 1: // step
        // TODO: solve ratio for texture index
        frag_color = stepn(texture(state, cor));
        break;
    default: // draw
        frag_color = texture(state, cor);
        break;
    }
}