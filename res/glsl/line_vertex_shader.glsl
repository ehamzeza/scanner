#version 400 core

in vec3 vertexPosition;
in vec3 vertexColor;

out vec3 passVertexColor;

uniform mat4 transformationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

void main(void) {
    gl_Position = projectionMatrix * viewMatrix * transformationMatrix * vec4(vertexPosition, 1.0);
    
    passVertexColor = vertexColor;
}
