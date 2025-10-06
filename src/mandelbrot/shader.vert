#version 460 core

layout (location = 0) in vec4 pos;

out vec2 cor;

void main() {
    gl_Position = vec4(pos.xy, 0, 1);
    cor = pos.zw;
}
