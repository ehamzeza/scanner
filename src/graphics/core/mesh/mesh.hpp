#pragma once

#include <vector>

class Mesh {
private:
	std::vector<float> vertices, normals, textures;
	std::vector<unsigned int> indices;
public:
	Mesh(const std::vector<float> vertices, const std::vector<float> normals, const std::vector<float> textures, const std::vector<unsigned int> indices);
	const unsigned int getVertexCount();
};
