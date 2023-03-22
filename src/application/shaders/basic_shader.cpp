#include "basic_shader.hpp"

BasicShader::BasicShader(){
    this->vertexPositionAttribute = 0;
    this->transformationMatrixLocation = 0;
}

void BasicShader::bindShaderAttributes() {
    this->vertexPositionAttribute = glGetAttribLocation(this->shaderProgramId, "vertexPosition");
}

void BasicShader::bindShaderUniforms() {
    // TODO: Abstract uniform binding.
    this->transformationMatrixLocation = glGetUniformLocation(this->shaderProgramId, "transformationMatrix");
    this->projectionMatrixLocation = glGetUniformLocation(this->shaderProgramId, "projectionMatrix");
    this->viewMatrixLocation = glGetUniformLocation(this->shaderProgramId, "viewMatrix");
    this->modelColor = glGetUniformLocation(this->shaderProgramId, "color");
}

void BasicShader::loadTransformationMatrix(const std::shared_ptr<glm::mat4 > transformationMatrix) {
    // TODO: Abstract matrix loading.
    float* matrixContents = glm::value_ptr(*(transformationMatrix.get())); // TODO: Remove casting.
    glUniformMatrix4fv(this->transformationMatrixLocation, 1, GL_FALSE, (const GLfloat*)matrixContents);
}

void BasicShader::loadProjectionMatrix(const std::shared_ptr<glm::mat4> projectionMatrix) {
    // TODO: Abstract matrix loading.
    float* matrixContents = glm::value_ptr(*(projectionMatrix.get())); // TODO: Remove casting.
    glUniformMatrix4fv(this->projectionMatrixLocation, 1, GL_FALSE, (const GLfloat*)matrixContents);
}

void BasicShader::loadViewMatrix(const std::shared_ptr<glm::mat4> viewMatrix) {
    // TODO: Abstract matrix loading.
    float* matrixContents = glm::value_ptr(*(viewMatrix.get())); // TODO: Remove casting.
    glUniformMatrix4fv(this->viewMatrixLocation, 1, GL_FALSE, (const GLfloat*)matrixContents);
}

void BasicShader::loadColor(float r, float g, float b) {
    glUniform3f(this->modelColor, r, g, b);
}
