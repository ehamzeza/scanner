#version 400 core

in vec3 passVertexColor;
out vec4 outColor;

void main(void) {
    outColor = vec4(passVertexColor, 1.0);
}
