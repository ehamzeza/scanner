#pragma once

#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>

#include <memory>

class MousePicker {
public:
    MousePicker();

    // TODO Comment:
    // Note: Returns a normalized vector.    
    glm::vec3 getMouseRay(const glm::vec2 mousePosition, const std::shared_ptr<glm::mat4> projectionMatrix, const std::shared_ptr<glm::mat4> viewMatrix);
};
