#version 460 core

out flat uint idx;

uniform uint coef_cnt;

void main() {
    idx = gl_VertexID;
}
