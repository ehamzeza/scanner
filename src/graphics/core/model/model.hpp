#pragma once

#include <array>
#include <iostream>
#include "buffers.hpp"

#include <glm/gtc/type_ptr.hpp>
#include "glm/mat4x4.hpp"

class Model {
protected:
	VAO vao;

public:
	glm::vec3 location;
	glm::vec3 rotation;
	glm::vec3 scale;

public:
	Model(VAO vao, glm::vec3 location, glm::vec3 rotation, glm::vec3 scale);
	Model(VAO vao, glm::vec3 location, glm::vec3 rotation, float scale);
	Model(VAO vao);
	Model(){};

	const VAO getVAO();

	void setScale(const float scale);

	glm::vec3 getLocation();
	glm::vec3 getRotation();
	glm::vec3 getScale();

	void render();
};
