#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 worldVertexPosition[3];

out vec3 normalVector;

void main(void) {
    // Compute the normal vector for the triangle we just recieved. It will
    // be the same for all of the points.
    vec3 first = worldVertexPosition[0] - worldVertexPosition[1];
    vec3 second = worldVertexPosition[2] - worldVertexPosition[1];
    normalVector = normalize(cross(first, second));

    // Although this can be done with a simple for-loop, the shader will
    // be much faster if we "unwrap" it like so.
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
}
