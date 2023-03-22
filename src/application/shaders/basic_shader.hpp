#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

#include "shaders.hpp"

class BasicShader : public ShaderProgram{
public:
	GLint vertexPositionAttribute;
	
	GLint transformationMatrixLocation;
	GLint projectionMatrixLocation;
	GLint viewMatrixLocation;

    GLint modelColor;

	BasicShader();

	void bindShaderAttributes() override;
	void bindShaderUniforms() override;

	void loadTransformationMatrix(const std::shared_ptr<glm::mat4 > transformationMatrix);
	void loadProjectionMatrix(const std::shared_ptr<glm::mat4> projectionMatrix);
	void loadViewMatrix(const std::shared_ptr<glm::mat4> viewMatrix);
    void loadColor(float r, float g, float b);
};
