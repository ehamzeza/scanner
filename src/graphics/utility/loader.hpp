#pragma once

#include <GL/glew.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "buffers.hpp"

class Loader {
public:
	const VAO createGridXZ(const float width, const float length, const float spacing);
	const VAO createCoordinate(const float size);
	const VAO createBasicTriangle();
	const VAO createBasicQuad();
	const VAO createFromOBJ(const std::string& filename);

private:
	void pushVector3f(std::vector<float>& vector, const float x, const float y, const float z, const int count = 1);
};
