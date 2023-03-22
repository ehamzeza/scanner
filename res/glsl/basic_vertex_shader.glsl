#version 440 core

in vec3 vertexPosition;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 worldVertexPosition;

void main(void) {
    gl_Position = projectionMatrix * viewMatrix * transformationMatrix * vec4(vertexPosition, 1.0);
    vec4 tmp = transformationMatrix * vec4(vertexPosition, 1.0);
    worldVertexPosition = tmp.xyz;
}
