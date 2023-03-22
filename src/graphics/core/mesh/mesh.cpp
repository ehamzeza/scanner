#include "mesh.hpp"

Mesh:: Mesh(const std::vector<float> vertices, const std::vector<float> normals, const std::vector<float> textures, const std::vector<unsigned int> indices) {
    this->vertices = vertices;
    this->normals = normals;
    this->textures = textures;
    this->indices = indices;
}

const unsigned int Mesh::getVertexCount() {
    return this->vertices.size();
}