#version 440 core

uniform vec3 color;

in vec3 normalVector;

out vec4 pixelColor;

const vec3 lightDirection = vec3(-1.5, -1.0, 1.25);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float minimumLight = 0.125;

void main(void) {
    // Note: The normal vector was normalized in the geometry shader. It MUST be normalized before the dot().
    float intensity = dot(normalize(lightDirection), normalVector);
    float diffuseBrightness = max(minimumLight, intensity);

    pixelColor = vec4(diffuseBrightness, diffuseBrightness, diffuseBrightness, 1.0) * vec4(color, 1.0);
}
