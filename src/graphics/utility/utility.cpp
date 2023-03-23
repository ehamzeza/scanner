#include "utility.hpp"

const std::string readFileContents(const std::string& filename) {
    std::ifstream file(filename);
	
    if(file.fail()) {
		std::cerr << "Failed to open file \"" << filename << "\"!" << std::endl;
		return "";
	}
	
    std::stringstream stream;
	std::string line;
	while (!file.eof()) {
		std::getline(file, line);
		stream << line << std::endl;
	}

	file.close();
	return stream.str();
}
