#pragma once

#include <GL/glew.h>
#include <iostream>
#include <vector>

// TODO: Comment OpenGL Data Structures

class VBO {
private:
	GLuint vboId;
	GLenum target;

public:
	VBO(const GLenum target);

	void bind();
	void unbind();
	void cleanUp();
};

class VAO {
public:
	GLuint vaoId;
	GLsizei renderCount;
	GLenum renderMode;
	bool indexed;

	std::vector<VBO> vbos;
	std::vector<int> attributes;

public:
	VAO();

	void create(const GLsizei vertexCount, GLenum renderMode);
	
	// Note: use drawElements() instead of drawArrays()
	void createIndexed(const std::vector<int> indices, GLenum renderMode);

	void bufferFloatData(const int attributeIndex, const int dimension, const std::vector<float> data);

	void bind();
	void unbind();
	void cleanUp();

	void render();

	const GLuint getId();
	const GLsizei getRenderCount();
};

class FBO {
private:
	int width;
	int height;

	GLuint fboId;
	GLuint colorTextureId;
	GLuint depthStencilId;

public:
	FBO();

	// Create doesn't bind the framebuffer.
	void create(const int width, const int height);

	// Uses color attachemnt index = 0.
	void addColorTextureAttachment();

	// Creates combinded depth-stencil buffer. Add this if planning on using GL_DEPTH test
	void addDepthStencilAttachment();

	void bind();
	void unbind();
	void cleanUp();

	GLuint getId();
	GLuint getColorTextureId();

	const int getWidth();
	const int getHeight();
};
