#include "model.hpp"

Model::Model(VAO vao, glm::vec3 location, glm::vec3 rotation, glm::vec3 scale) {
	this->vao = vao;
	this->location = location;
	this->rotation = rotation;
	this->scale = scale;
}

Model::Model(VAO vao, glm::vec3 location, glm::vec3 rotation, float scale) {
	this->vao = vao;
	this->location = location;
	this->rotation = rotation;
	this->scale = glm::vec3(scale, scale, scale);
}

Model::Model(VAO vao) {
	this->vao = vao;
	this->location = glm::vec3(0.0, 0.0, 0.0);
	this->rotation = glm::vec3(0.0, 0.0, 0.0);
	this->scale = glm::vec3(1.0, 1.0, 1.0);	
}

void Model::setScale(const float scale) {
	this->scale = glm::vec3(scale, scale, scale);
}

const VAO Model::getVAO() {
	return this->vao;
}

glm::vec3 Model::getLocation(){
	return this->location;
}

glm::vec3 Model::getRotation(){
	return this->rotation;
}

glm::vec3 Model::getScale(){
	return this->scale;
}

void Model::render() {
	this->vao.render();
}
