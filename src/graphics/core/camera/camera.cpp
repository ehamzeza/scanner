#include "camera.hpp"

Camera::Camera(glm::vec3 location, glm::vec3 rotation) {
    this->location = location;
    this->rotation = rotation;
}

Camera::Camera() {
    this->location = glm::vec3(0.0f, 0.0f, 0.0f);
    this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 Camera::getLocation(){
    return this->location;
}

glm::vec3 Camera::getRotation(){
    return this->rotation;
}
