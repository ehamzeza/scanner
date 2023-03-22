#include "math.hpp"

glm::vec3 intersectRayOnPlane(const glm::vec3 rayPoint, const glm::vec3 rayDirection, const glm::vec3 planePoint, const glm::vec3 planeNormalVector) {
    // The dot product returns how similar vectors are. If the dot product is zero, that
    // means that the vectors are perpandicular. If that is the case between the plane's
    // normal vector and the ray direction, then an intersection cannot exist. Note the
    // vectors MUST be unit vectors.
    float directionDot = glm::dot(glm::normalize(planeNormalVector), glm::normalize(rayDirection));

    // Test to see if an intersection can exist. If not, return the origin.
    if (fabsf(directionDot) < EPSILON) {
        return glm::vec3 {0.0, 0.0, 0.0};
    }

    // Determine the point at which the vectors intersect. This is done as recommended
    // by the Stack Overflow post: https://stackoverflow.com/a/39424162
    glm::vec3 delta = rayPoint - planePoint;
    float sigma = -glm::dot(planeNormalVector, delta) / directionDot;
    glm::vec3 intersectionPoint = (sigma * rayDirection) + delta + planePoint;
    return intersectionPoint;
}

const std::shared_ptr<glm::mat4> computeTransformationMatrix(Model model) {
    glm::mat4 identity = glm::mat4(1.0);
    
    glm::mat4 translated = glm::translate(identity, model.getLocation());

    glm::vec3 modelRotation = model.getRotation();
    glm::mat4 rotatedX = glm::rotate(translated, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotatedY = glm::rotate(rotatedX, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotatedZ = glm::rotate(rotatedY, modelRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 transformationMatrix = glm::scale(rotatedZ, model.getScale());

    return std::make_shared<glm::mat4> (transformationMatrix);
}

const std::shared_ptr<glm::mat4> computeViewMatrix(Camera camera) {
    glm::mat4 identity = glm::mat4(1.0);

    glm::vec3 cameraOrientation = camera.getRotation();
    glm::mat4 rotatedX = glm::rotate(identity, -cameraOrientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotatedY = glm::rotate(rotatedX, -cameraOrientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotatedZ = glm::rotate(rotatedY, -cameraOrientation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 translated = glm::translate(rotatedZ, -camera.getLocation());

    return std::make_shared<glm::mat4> (translated);
}

const std::shared_ptr<glm::mat4> computeProjectionMatrix(const float aspectRatio, const float fov) {
    glm::mat4 projectionMatrix = glm::perspective(fov, aspectRatio, 0.01f, 1000.0f);
    return std::make_shared<glm::mat4> (projectionMatrix);
}

float clipValue(float input, const float min, const float max) {
    input = input > max ? max : input;
    input = input < min ? min : input;
    return input;
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& vector){
    os << std::setprecision(2) << std::fixed;
    os << "[" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& vector){
    os << std::setprecision(2) << std::fixed;
    os << "[" << vector.x << ", " << vector.y << ", " << vector.z << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec2& vector){
    os << std::setprecision(2) << std::fixed;
    os << "[" << vector.x << ", " << vector.y << "]";
    return os;
}
