#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 i_col;

out vec3 col;

void main() {
    gl_Position = vec4(pos, 1);
    col = i_col;
}
