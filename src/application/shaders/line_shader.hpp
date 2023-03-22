#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

#include "shaders.hpp"

class LineShader : public ShaderProgram {
public:
    GLint vertexPositionAttribute;
    GLint vertexColorAttribute;
    	
	GLint transformationMatrixLocation;
	GLint projectionMatrixLocation;
	GLint viewMatrixLocation;

    LineShader();

    void bindShaderAttributes() override;
    void bindShaderUniforms() override;

    void loadTransformationMatrix(const std::shared_ptr<glm::mat4 > transformationMatrix);
	void loadProjectionMatrix(const std::shared_ptr<glm::mat4> projectionMatrix);
	void loadViewMatrix(const std::shared_ptr<glm::mat4> viewMatrix);
};
