#include "line_shader.hpp"

LineShader::LineShader() {
    this->vertexPositionAttribute = 0;
}

void LineShader::bindShaderAttributes() {
    this->vertexPositionAttribute = glGetAttribLocation(this->shaderProgramId, "vertexPosition");
    this->vertexColorAttribute = glGetAttribLocation(this->shaderProgramId, "vertexColor");
}

void LineShader::bindShaderUniforms() {
    // TODO: Abstract uniform binding.
    this->transformationMatrixLocation = glGetUniformLocation(this->shaderProgramId, "transformationMatrix");
    this->projectionMatrixLocation = glGetUniformLocation(this->shaderProgramId, "projectionMatrix");
    this->viewMatrixLocation = glGetUniformLocation(this->shaderProgramId, "viewMatrix");
}

void LineShader::loadTransformationMatrix(const std::shared_ptr<glm::mat4 > transformationMatrix) {
    // TODO: Abstract matrix loading.
    float* matrixContents = glm::value_ptr(*(transformationMatrix.get())); // TODO: Remove casting.
    glUniformMatrix4fv(this->transformationMatrixLocation, 1, GL_FALSE, (const GLfloat*)matrixContents);
}

void LineShader::loadProjectionMatrix(const std::shared_ptr<glm::mat4> projectionMatrix) {
    // TODO: Abstract matrix loading.
    float* matrixContents = glm::value_ptr(*(projectionMatrix.get())); // TODO: Remove casting.
    glUniformMatrix4fv(this->projectionMatrixLocation, 1, GL_FALSE, (const GLfloat*)matrixContents);
}

void LineShader::loadViewMatrix(const std::shared_ptr<glm::mat4> viewMatrix) {
    // TODO: Abstract matrix loading.
    float* matrixContents = glm::value_ptr(*(viewMatrix.get())); // TODO: Remove casting.
    glUniformMatrix4fv(this->viewMatrixLocation, 1, GL_FALSE, (const GLfloat*)matrixContents);
}
