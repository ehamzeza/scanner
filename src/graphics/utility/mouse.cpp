#include "mouse.hpp"

MousePicker::MousePicker() {
    
}

glm::vec3 MousePicker::getMouseRay(const glm::vec2 mousePosition, const std::shared_ptr<glm::mat4> projectionMatrix, const std::shared_ptr<glm::mat4> viewMatrix) {
    // Compute the required inverse matrices, these will be used later.
    glm::mat4 inverseProjectionMatrix = glm::inverse(*projectionMatrix);
    glm::mat4 inverseViewMatrix = glm::inverse(*viewMatrix);
    
    // First convert the 2D mouse position into a vector that points "into" the screen.
    // This vector is called the "clip-space"
    glm::vec4 clipSpaceVector = glm::vec4 {mousePosition.x, mousePosition.y, -1.0, 1.0};

    // Step 1: Convert the screen-space vector into eye-space with the projection matrix
    glm::vec4 eyeSpaceVector = inverseProjectionMatrix * clipSpaceVector;

    // After converting to eye space, we need to make sure that the vector is pointing
    // into the screen, as well as remove the unecessary component.
    eyeSpaceVector.z = -1.0;
    eyeSpaceVector.w = 0.0;

    // Step 2: Convert the eye-space vector into a world-space vector using the view matrix.
    glm::vec4 worldSpaceVector = inverseViewMatrix * eyeSpaceVector;

    // Finally, return a normalized version of the world space vector.
    return glm::normalize(glm::vec3{worldSpaceVector.x, worldSpaceVector.y, worldSpaceVector.z});
}
