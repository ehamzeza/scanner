#pragma once

#include <GL/glew.h>
#include <iostream>
#include <string>

#include "utility.hpp"

// TODO: Comment ShaderProgram class, using VS Code templates.
class ShaderProgram {
protected:
	std::string vertexShaderSource;
	std::string geometryShaderSource;
	std::string fragmentShaderSource;

public:
	GLuint shaderProgramId;
	GLuint vertexShaderId;
	GLuint geometryShaderId;
	GLuint fragmentShaderId;

	ShaderProgram();

	void createProgram(const std::string& vertexFilepath, const std::string& fragmentFilepath);
	void createProgram(const std::string& vertexFilepath, const std::string& geometryFilepath, const std::string& fragmentFilepath);

	void startShader();
	void stopShader();
	void cleanUpShader();

protected:
	virtual void bindShaderAttributes() = 0;
	virtual void bindShaderUniforms() = 0;

private:
	GLuint createShader(const std::string& shaderSource, const std::string& shaderFilepath, GLint shaderType);
};
