#pragma once

#include <iostream>

#include "basic_shader.hpp"
#include "line_shader.hpp"

#include "viewport.hpp"
#include "shaders.hpp"
#include "surface.hpp"
#include "buffers.hpp"
#include "camera.hpp"
#include "window.hpp"
#include "loader.hpp"
#include "model.hpp"
#include "mouse.hpp"
#include "math.hpp"

class Application {
private:
	bool runApplication;
	int exitStatus;

protected:
	Window* window;
	Loader loader;
	
	Viewport3D viewport;
	VAO coordVAO;
	VAO cubeVAO;
	VAO gridVAO;
	
public:
	Application();

	const int startApplication(const int width, const int height);

	void renderApplicationGUI();

	void cleanUpAplication();

	const void terminateApplication(const int exitStatus = 0);
};
