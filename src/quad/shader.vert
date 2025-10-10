#version 460 core

layout (location = 0) in vec2 pos;

uniform mat4 proj;

void main() {
    gl_Position = proj * vec4(pos.xy, 0, 1);
}