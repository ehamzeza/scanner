#pragma once

#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>
#include <bits/stdc++.h>

#include <memory>
#include <array>

#include "surface.hpp"
#include "camera.hpp"
#include "model.hpp"

// Define the threshold at which we are going to consider float equalities at.
#define EPSILON 1e-6

glm::vec3 intersectRayOnPlane(const glm::vec3 rayPoint, const glm::vec3 rayDirection, const glm::vec3 planePoint, const glm::vec3 planeNormalVector);

const std::shared_ptr<glm::mat4> computeTransformationMatrix(Model model);

const std::shared_ptr<glm::mat4> computeViewMatrix(Camera camera);

const std::shared_ptr<glm::mat4> computeProjectionMatrix(const float aspectRatio, const float fov);

const std::shared_ptr<glm::mat4> computeProjectionMatrix(GLSurface& surface);

float clipValue(float input, const float min, const float max);

std::ostream& operator<<(std::ostream& os, const glm::vec4& vector);

std::ostream& operator<<(std::ostream& os, const glm::vec3& vector);

std::ostream& operator<<(std::ostream& os, const glm::vec2& vector);
