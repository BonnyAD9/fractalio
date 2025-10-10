#version 460 core

layout (location = 0) in vec4 pos;

uniform mat4 proj;

out highp vec2 cor;

void main() {
    gl_Position = proj * vec4(pos.xy, 0, 1);
    cor = pos.zw;
}