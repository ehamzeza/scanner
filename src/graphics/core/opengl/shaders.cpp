#include "shaders.hpp"

ShaderProgram::ShaderProgram() {
	this->shaderProgramId = 0;
	this->vertexShaderId = 0;
	this->fragmentShaderId = 0;
}

void ShaderProgram::createProgram(const std::string& vertexFilepath, const std::string& fragmentFilepath) {
	this->vertexShaderSource = readFileContents(vertexFilepath);
	this->fragmentShaderSource = readFileContents(fragmentFilepath);

	this->vertexShaderId = this->createShader(this->vertexShaderSource, vertexFilepath, GL_VERTEX_SHADER);
	this->fragmentShaderId = this->createShader(this->fragmentShaderSource, fragmentFilepath, GL_FRAGMENT_SHADER);

	this->shaderProgramId = glCreateProgram();
	glAttachShader(this->shaderProgramId, this->vertexShaderId);
	glAttachShader(this->shaderProgramId, this->fragmentShaderId);
	glLinkProgram(this->shaderProgramId);

	this->bindShaderUniforms();
	this->bindShaderAttributes();
}

void ShaderProgram::createProgram(const std::string& vertexFilepath, const std::string& geometryFilepath, const std::string& fragmentFilepath) {
	this->vertexShaderSource = readFileContents(vertexFilepath);
	this->geometryShaderSource = readFileContents(geometryFilepath);
	this->fragmentShaderSource = readFileContents(fragmentFilepath);

	this->vertexShaderId = this->createShader(this->vertexShaderSource, vertexFilepath, GL_VERTEX_SHADER);
	this->geometryShaderId = this->createShader(this->geometryShaderSource, geometryFilepath, GL_GEOMETRY_SHADER);
	this->fragmentShaderId = this->createShader(this->fragmentShaderSource, fragmentFilepath, GL_FRAGMENT_SHADER);
	this->shaderProgramId = glCreateProgram();

	glAttachShader(this->shaderProgramId, this->vertexShaderId);
	glAttachShader(this->shaderProgramId, this->geometryShaderId);
	glAttachShader(this->shaderProgramId, this->fragmentShaderId);
	glLinkProgram(this->shaderProgramId);

	this->bindShaderUniforms();
	this->bindShaderAttributes();
}

GLuint ShaderProgram::createShader(const std::string& shaderSource, const std::string& shaderFilepath, GLint shaderType) {
	GLuint shaderId = glCreateShader(shaderType);

	const char* shaderSourceString = shaderSource.c_str();
	glShaderSource(shaderId, 1, &shaderSourceString, 0);
	glCompileShader(shaderId);

	// Check the chader compilation for errors, if there are any
	// print them to the standard error stream 
	GLint compileResult = GL_TRUE;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileResult);

    if (compileResult == GL_FALSE) {
		// Determine the length of the shader log so that we can store
		// it appropriately
		GLint logLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

		// Create a string buffer long enough to contain the entire log
		// and the null-terminator.
		logLength += 1; // Increase the buffer size for the null-character.
		GLchar* infoLog = new GLchar[logLength];
    	GLsizei storedLogLength = 0;
    	glGetShaderInfoLog(shaderId, logLength, &storedLogLength, infoLog);

		// Print and delete the buffer used to store the log
		std::cerr << "Failed to compile shader \"" << shaderFilepath << "\" Shader Info Log:" << std::endl;
        std::cerr << infoLog << std::endl;
        delete[] infoLog;
        
        // Delete the partially created shader.
        glDeleteShader(shaderId);
        return -1;
	}

	return shaderId;
}

void ShaderProgram::startShader() {
	glUseProgram(this->shaderProgramId);
}

void ShaderProgram::stopShader() {
	glUseProgram(0);
}

void ShaderProgram::cleanUpShader() {
	// Make sure the shader isn't running.
	this->stopShader();

	// For each of the shader types, delete it if it was created.
	if (this->vertexShaderId != 0) {
		glDetachShader(this->shaderProgramId, this->vertexShaderId);
		glDeleteShader(this->vertexShaderId);
	}
	if (this->geometryShaderId != 0) {
		glDetachShader(this->shaderProgramId, this->geometryShaderId);
		glDeleteShader(this->geometryShaderId);
	}
	if (this->fragmentShaderId != 0) {
		glDetachShader(this->shaderProgramId, this->fragmentShaderId);
		glDeleteShader(this->fragmentShaderId);
	}

	// Finally, delete the program.
	if (this->shaderProgramId != 0) {
		glDeleteProgram(this->shaderProgramId);
	}
}