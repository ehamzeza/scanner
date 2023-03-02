#include "buffers.hpp"

VBO::VBO(const GLenum target) {
	this->target = target;
	glGenBuffers(1, &this->vboId);
}

void VBO::bind() {
	glBindBuffer(this->target, this->vboId);
}

void VBO::unbind() {
	glBindBuffer(this->target, 0);
}

void VBO::cleanUp() {
	glDeleteBuffers(1, &this->vboId);
}

VAO::VAO() {
	// Zero is OpenGL's default VAO id which means if we bind
	// this uninitialized class, nothing will happen.
	this->vaoId = 0;
	this->renderCount = 0;
}

void VAO::create(const GLsizei vertexCount, GLenum renderMode) {
	this->renderCount = vertexCount;
	this->renderMode = renderMode;
	this->indexed = false;

	glGenVertexArrays(1, &this->vaoId);
	glBindVertexArray(this->vaoId);
}

void VAO::createIndexed(const std::vector<int> indices, GLenum renderMode) {
	this->renderCount = indices.size();
	this->renderMode = renderMode;
	this->indexed = true;

	glGenVertexArrays(1, &this->vaoId);
	glBindVertexArray(this->vaoId);

	// Create the index buffer that OpenGL will use for
	// Indexed rendering.

	VBO indicesVBO(GL_ELEMENT_ARRAY_BUFFER);
	this->vbos.push_back(indicesVBO);

	indicesVBO.bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
	// Note: DO NOT unbind the indices buffer. It MUST be bound while
	// the other buffers are being created so they can be indexed.
}

void VAO::bufferFloatData(const int attributeIndex, const int dimension, const std::vector<float> data) {
	this->attributes.push_back(attributeIndex);

	VBO vbo(GL_ARRAY_BUFFER);
	this->vbos.push_back(vbo);

	vbo.bind();
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(attributeIndex, dimension, GL_FLOAT, GL_FALSE, dimension * sizeof(float), (void*)0);
	vbo.unbind();
}

void VAO::bind() {
	glBindVertexArray(this->vaoId);

	// Enable all of the attributes that this VAO uses.
	for(const int& attribute : this->attributes) {
		glEnableVertexAttribArray(attribute);
	}
}

void VAO::unbind() {
	// Disable all of the attributes that this VAO uses.
	for(const int& attribute : this->attributes) {
		glDisableVertexAttribArray(attribute);
	}
	
	glBindVertexArray(0);
}

void VAO::render() {
	this->bind();
	if(this->indexed) {
		glDrawElements(this->renderMode, this->getRenderCount(), GL_UNSIGNED_INT, (void*) 0);
	} else {
		glDrawArrays(this->renderMode, 0, this->getRenderCount());
	}
	this->unbind();
}

const GLuint VAO::getId() {
	return this->vaoId;
}

const GLsizei VAO::getRenderCount() {
	return this->renderCount;
}

void VAO::cleanUp() {
	// Delete any VBO objects that this VAO uses.
	for (VBO& vbo : this->vbos) {
		vbo.cleanUp();
	}

	// Delete this VAO if it is initialized to be non-default.
	if (this->vaoId != 0) {
		glDeleteBuffers(1, &this->vaoId);
	}
}

FBO::FBO() {
	this->width = 0;
	this->height = 0;
	this->fboId = 0;
	this->colorTextureId = 0;
	this->depthStencilId = 0;
}

void FBO::create(const int width, const int height) {
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &this->fboId);
}

void FBO::addColorTextureAttachment() {
	this->bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glGenTextures(1, &this->colorTextureId);
	glBindTexture(GL_TEXTURE_2D, this->colorTextureId);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*) 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->colorTextureId, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	this->unbind();
}

void FBO::addDepthStencilAttachment() {
	this->bind();

	glGenTextures(1, &this->depthStencilId);
	glBindTexture(GL_TEXTURE_2D, this->depthStencilId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, this->width, this->height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, (void*) 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, this->depthStencilId, 0);

	glBindTexture(GL_TEXTURE_2D, 0);	
	this->unbind();
}

void FBO::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, this->fboId);
}

void FBO::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::cleanUp() {
	// Delete any attachemnts if they were created.
	if (this->colorTextureId != 0) {
		glDeleteTextures(1, &this->colorTextureId);
	}

	if (this->depthStencilId != 0) {
		glDeleteTextures(1, &this->depthStencilId);
	}

	// Delete the FBO only if it was created (therefore, non-zero).
	if(this->fboId != 0) {
		glDeleteFramebuffers(1, &this->fboId);
	}
}

GLuint FBO::getId(){
	return this->fboId;
}

GLuint FBO::getColorTextureId(){
	return this->colorTextureId;
}

const int FBO::getWidth() {
	return this->width;
}

const int FBO::getHeight() {
	return this->height;
}