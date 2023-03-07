#pragma once

#include <iostream>
#include "glm/mat4x4.hpp"

class Camera {
public:
	glm::vec3 location;
	glm::vec3 rotation;

public:
    Camera(glm::vec3 location, glm::vec3 rotation);

    // Creates a camera positioned at (0, 0, 0) facing directly towards -Z.
    Camera();

    glm::vec3 getLocation();
    glm::vec3 getRotation();
};
