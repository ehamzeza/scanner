#include "loader.hpp"

const VAO Loader::createGridXZ(const float width, const float length, const float spacing) {
	std::vector<float> vertices;
	std::vector<float> colors;

	// First create the outer grid, this will be a lighter color
	pushVector3f(vertices, -width / 2.0, 0.0, -length / 2.0);
	pushVector3f(vertices, width / 2.0, 0.0, -length / 2.0);
	pushVector3f(vertices, -width / 2.0, 0.0, length / 2.0);
	pushVector3f(vertices, width / 2.0, 0.0, length / 2.0);
	pushVector3f(vertices, -width / 2.0, 0.0, -length / 2.0);
	pushVector3f(vertices, -width / 2.0, 0.0, length / 2.0);
	pushVector3f(vertices, width / 2.0, 0.0, -length / 2.0);
	pushVector3f(vertices, width / 2.0, 0.0, length / 2.0);
	pushVector3f(colors, 1.0, 1.0, 1.0, 8);

	// Now create the inner grid following the spacing value.
	for (float pos = (-width / 2.0) + spacing; pos < width / 2.0; pos += spacing) {
		pushVector3f(vertices, pos, 0.0, -length / 2.0);
		pushVector3f(vertices, pos, 0.0, length / 2.0);
		pushVector3f(colors, 0.75, 0.75, 0.75, 2);
	}

	for (float pos = (-length / 2.0) + spacing; pos < length / 2.0; pos += spacing) {
		pushVector3f(vertices, -width / 2.0, 0.0, pos);
		pushVector3f(vertices, width / 2.0, 0.0, pos);
		pushVector3f(colors, 0.75, 0.75, 0.75, 2);
	}

	VAO vao;
	vao.create(vertices.size() / 2, GL_LINES);
	vao.bufferFloatData(0, 3, vertices);
	vao.bufferFloatData(1, 3, colors);
	return vao;
}

const VAO Loader::createCoordinate(const float size) {
	std::vector<float> vertices = {
		// Basic XYZ coordinate lines for now.
		0.0, 0.0, 0.0,
		size, 0.0, 0.0,

		0.0, 0.0, 0.0,
		0.0, size, 0.0,

		0.0, 0.0, 0.0,
		0.0, 0.0, size	
	};

	std::vector<float> colors = {
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,

		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,

		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0
	};

	VAO vao;
	vao.create(vertices.size() / 2, GL_LINES);
	vao.bufferFloatData(0, 3, vertices);
	vao.bufferFloatData(1, 3, colors);
	return vao;
}

const VAO Loader::createBasicTriangle() {
	std::vector<float> vertices = {
		0.0, 0.5, 0.0,
		0.5, -0.5, 0.0,
		-0.5, -0.5, 0.0
	};

	std::vector<int> indices = {
		0, 1, 2
	};

	VAO vao;
	vao.createIndexed(indices, GL_TRIANGLES);
	vao.bufferFloatData(0, 3, vertices);
	return vao;
}

const VAO Loader::createBasicQuad() {
	std::vector<float> vertices = {
		-0.5, 0.5, 0.0,
		0.5, 0.5, 0.0,
		0.5, -0.5, 0.0,
		-0.5, -0.5, 0.0
	};

	std::vector<int> indices = {
		0, 1, 2,
		0, 2, 3
	};

	VAO vao;
	vao.createIndexed(indices, GL_TRIANGLES);
	vao.bufferFloatData(0, 3, vertices);
	return vao;
}

const VAO Loader::createFromOBJ(const std::string& filename) {
	std::vector<float> vertices;
	std::vector<int> indices;

	std::ifstream file(filename);
	if(file.fail()) {
		std::cerr << "Failed to open file \"" << filename << "\"!" << std::endl;
		return VAO();
	}

	std::string line;
	while (!file.eof()) {
		std::getline(file, line);
		
		// Remove comment lines which start with "#"
		if(line.rfind("#", 0) == 0) {
			continue;
		}

		// Create a string stream of the line, this will be used for parsing later.
		std::stringstream stream;
		stream << line;

		// Temporary buffer we can use for manipulating the read line.
		std::string temp;

		// If the line contains a vertex, simply append it to our vertex array.
		if(line.rfind("v ", 0) == 0) {
			stream >> temp; // Throw away the "v " at the beginning of the line.

			float x, y, z;
			stream >> x >> y >> z;	
			vertices.push_back(x);	
			vertices.push_back(y);	
			vertices.push_back(z);
			continue;
		}

		if (line.rfind("f ", 0) == 0) {
			stream >> temp; // Throw away the "f " at the beginning of the line.
			
			// There is an unknown number of vertices on this face, loop till
			// we've hit the end of the line.
			std::vector<std::string> faceVertices;
			while (!stream.eof()) {
				stream >> temp;
				faceVertices.push_back(temp);
			}

			// Currently, the program can only handle triangulated faces, print a warning
			// if we encounter something non triangular.
			if (faceVertices.size() != 3) {
				std::cout << "Warning: Input File \"" << filename << "\" is not triangulated! It may not render correctly" << std::endl;
			}

			// We are only interested in the "index" part of each face vertex information,
			// loop and add this information to our indices.
			int index = 0;
			for (std::string faceVertex : faceVertices) {
				std::stringstream ss(faceVertex);
				ss >> index;

				indices.push_back(index - 1); // OBJ files index starting at 1
			}

			continue;
		}
	}

	file.close();

	VAO vao;
	vao.createIndexed(indices, GL_TRIANGLES);
	vao.bufferFloatData(0, 3, vertices);
	return vao;
}

void Loader::pushVector3f(std::vector<float>& vector, const float x, const float y, const float z, const int count) {
	for (int i = 0; i < count; i++) {
		vector.push_back(x);
		vector.push_back(y);
		vector.push_back(z);
	}
}
