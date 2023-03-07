#include "viewport.hpp"

Viewport3D::Viewport3D() {
    this->surfaceProjectionMatrix = std::make_shared<glm::mat4>(1.0);
    this->surfaceViewMatrix = std::make_shared<glm::mat4>(1.0);
}

void Viewport3D::createViewport(const std::string title, const int renderWidth, const int renderHeight) {
	this->title = title;

    // Create all of the necessary shaders used for rendering in this viewport:
   	this->basicShader.createProgram("../res/glsl/basic_vertex_shader.glsl", 
								     "../res/glsl/basic_geometry_shader.glsl",
								     "../res/glsl/basic_fragment_shader.glsl");
	this->lineShader.createProgram("../res/glsl/line_vertex_shader.glsl",
									"../res/glsl/line_fragment_shader.glsl");

    // Create the FBO "surface" that will handle the backend rendering.
	this->surfaceAspectRatio = 1.0;
	this->surfaceFBO.create(renderWidth, renderHeight);
	this->surfaceFBO.addDepthStencilAttachment();
	this->surfaceFBO.addColorTextureAttachment();

    // Set the starting point of the camera in the 3D space.
	this->orbit.distance = 10.0f;
	this->orbit.orientation = -M_PI / 4.0f;
	this->orbit.angle = M_PI / 6.0f;
}

void Viewport3D::updateViewport() {
    this->surfaceProjectionMatrix = computeProjectionMatrix(this->surfaceAspectRatio, 70.0f);
    this->surfaceViewMatrix = computeViewMatrix(this->camera);

    this->updateOrbitalCamera();
}

void Viewport3D::bindAsRenderTarget() {
	// Bind the FBO to be the current render target
	this->surfaceFBO.bind();
	glViewport(0, 0, this->surfaceFBO.getWidth(), this->surfaceFBO.getHeight());
}

void Viewport3D::clearViewport() {
	glEnable(GL_DEPTH_TEST);
    glClearColor(0.125, 0.125, 0.125, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Viewport3D::renderLines(Model& model, const float lineWidth) {
    this->lineShader.startShader();

    std::shared_ptr<glm::mat4> transformationMatrix = computeTransformationMatrix(model);
    this->lineShader.loadTransformationMatrix(transformationMatrix);
    this->lineShader.loadProjectionMatrix(this->surfaceProjectionMatrix);
    this->lineShader.loadViewMatrix(this->surfaceViewMatrix);
    glLineWidth(lineWidth);
    model.render();

    this->lineShader.stopShader();
}

void Viewport3D::renderModel(Model& model, const glm::vec3 color) {
    this->basicShader.startShader();
    this->basicShader.loadColor(color.r, color.g, color.b);
    
    std::shared_ptr<glm::mat4> transformationMatrix = computeTransformationMatrix(model);
    this->basicShader.loadTransformationMatrix(transformationMatrix);
    this->basicShader.loadProjectionMatrix(this->surfaceProjectionMatrix);
    this->basicShader.loadViewMatrix(this->surfaceViewMatrix);
    model.render();

    this->basicShader.stopShader();
}

void Viewport3D::handleInput() {
	ImGuiIO* io = &ImGui::GetIO();
	
	/**
	 * Handle mouse scrolling, basically just change the distance parameter depending
	 * on the mouse wheel delta.
	 */
	this->orbit.distance -= io->MouseWheel;

	/**
	 * Handle the rotation and panning of the mouse. This needs to be done in one
	 * operation because we wish to prefferentially pan when the user chooses 
	 * both operations.
	 */

	// Determine the required information for the far plane. The far plane
	// is "parallel" to the camera's viewpoint.
	glm::vec4 negativeZ = {0.0, 0.0, -1.0, 1.0};
	glm::vec4 cameraNormal4 = glm::inverse(*this->surfaceViewMatrix) * negativeZ;
	glm::vec3 farPlaneNormal = glm::normalize((glm::vec3{cameraNormal4.x, cameraNormal4.y, cameraNormal4.z} / cameraNormal4.w) - this->camera.getLocation());
	glm::vec3 farPlanePosition = this->camera.getLocation() + (farPlaneDistance * farPlaneNormal);

	// Determine the mouse ray from the currenyt camera position.
	glm::vec3 mouseRay = this->mousePicker.getMouseRay(this->getMouseLocation(), this->surfaceProjectionMatrix, this->surfaceViewMatrix);
	
	// If either mouse button was clicked, save the current orbital data. It will be
	// needed in computing the new orbit data given the current mouse offset.
	if (io->MouseClicked[0] || io->MouseClicked[1]) {
		this->mouseDownOrbit = this->orbit;

		// Save the location of the camera before panning occured.
		this->mouseDownCameraLocation = this->camera.getLocation();
		// Save the location of the point we're panning on. I.e the point the mouse ray is intersecting with.
		this->mouseDownIntersection = intersectRayOnPlane(camera.getLocation(), mouseRay, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0});

		// Save the location of the mouse on the far plane.
		this->mouseDownFarPlane = intersectRayOnPlane(camera.getLocation(), mouseRay, farPlanePosition, farPlaneNormal);
	}

	// Determine the size, and relative top-left corner location of the "surface".
	ImVec2 imWindowSize = ImGui::GetWindowSize();
	ImVec2 imCursorPosition = ImGui::GetCursorScreenPos();

	if (io->MouseDown[0]) {
		// Determine if the mouse was pressed while inside the viewport window.
		ImVec2 mouseDownPosition = io->MouseClickedPos[0];
		ImVec2 relativeMousePosition = ImVec2(mouseDownPosition.x - imCursorPosition.x, mouseDownPosition.y - imCursorPosition.y);
		bool pressedInsideWindow = relativeMousePosition.x < imWindowSize.x && relativeMousePosition.y < imWindowSize.y && relativeMousePosition.x >= 0 && relativeMousePosition.y >= 0;

		// If the mouse wasn't pressed inside the window, do not continue.
		if(!pressedInsideWindow) {
			return;
		}

		// Determine the cursor's current position on the "far plane". 
		glm::vec3 currentFarPlane = intersectRayOnPlane(camera.getLocation(), mouseRay, farPlanePosition, farPlaneNormal);

		// If the current far plane position is the same as the mouse down position, then there
		// is no panning to do right now. To avoid dividing by zero ignore this frame.
		if(this->mouseDownFarPlane != currentFarPlane) {
			// Now we are going to determine the distance to move the focal point using the law of similar triangles.
			// Triangle 1: mouseDownIntersection-mouseDownCameraLocation-mouseDownFarPlane
			// Triangle 2: mouseDownIntersection-?currentCameraLocation?-currentFarPlane

			glm::vec3 direction = -glm::normalize(currentFarPlane - this->mouseDownFarPlane);
			float magnitude = glm::length(this->mouseDownCameraLocation - this->mouseDownIntersection) * 
			(glm::length(this->mouseDownFarPlane - currentFarPlane) /
			(glm::length(this->mouseDownFarPlane - this->mouseDownIntersection)));

			// Determine how much to move the focal point and adjust it accordingly.		
			glm::vec3 deltaFocus = magnitude * direction;
			this->orbit.focusX = this->mouseDownOrbit.focusX + deltaFocus.x;
			this->orbit.focusY = this->mouseDownOrbit.focusY + deltaFocus.y;
			this->orbit.focusZ = this->mouseDownOrbit.focusZ + deltaFocus.z;
			this->updateOrbitalCamera();
		}
	} else if (io->MouseDown[1]) {
		// Determine if the mouse was pressed while inside the viewport window.
		ImVec2 mouseDownPosition = io->MouseClickedPos[1];
		ImVec2 relativeMousePosition = ImVec2(mouseDownPosition.x - imCursorPosition.x, mouseDownPosition.y - imCursorPosition.y);
		bool pressedInsideWindow = relativeMousePosition.x < imWindowSize.x && relativeMousePosition.y < imWindowSize.y && relativeMousePosition.x >= 0 && relativeMousePosition.y >= 0;

		// If the mouse wasn't pressed inside the window, do not continue.
		if(!pressedInsideWindow) {
			return;
		}

		// The right mouse button is down, we should angle / orbit the camera.
		ImVec2 dragDeltaRight = ImGui::GetMouseDragDelta(1);

		this->orbit.angle = clipValue(this->mouseDownOrbit.angle + (0.005 * dragDeltaRight.y), 0.0f, M_PI / 2.0f);
		this->orbit.orientation = this->mouseDownOrbit.orientation - (0.005 * dragDeltaRight.x);
		this->updateOrbitalCamera();
	}
	
}

void Viewport3D::RenderImGui() {
    // Render the ImGUI front end that will display the contents of the FBO.
	ImGui::Begin(this->title.c_str());
    ImGui::BeginChild(std::string(this->title + " Child").c_str());
    ImVec2 windowSize = ImGui::GetWindowSize();
    this->surfaceAspectRatio = (float) windowSize.x / (float) windowSize.y;

	// Before rendering the image, handle any mouse input related to the child object.
	this->handleInput();

	// OpenGL's texture origin is different than ImGui's, it needs to be inverted.
    ImGui::Image((ImTextureID)this->surfaceFBO.getColorTextureId(), windowSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
    ImGui::EndChild();
	ImGui::End();


	// Render the Camera Control Sliders.
	ImGui::Begin("Camera Control");
	ImGui::SliderFloat("Distance", &this->orbit.distance, 1, 15, "%.3f", 0);
	ImGui::SliderFloat("Focus Z", &this->orbit.focusZ, -10, 10, "%.3f", 0);
	ImGui::SliderFloat("Focus X", &this->orbit.focusX, -10, 10, "%.3f", 0);
	ImGui::SliderFloat("Focus Y", &this->orbit.focusY, -10, 10, "%.3f", 0);
	ImGui::SliderFloat("Orienation", &this->orbit.orientation, -M_PI, M_PI, "%.3f", 0);
	ImGui::SliderFloat("Angle", &this->orbit.angle, -0, M_PI / 2.0, "%.3f", 0);
	ImGui::End();
}

glm::vec2 Viewport3D::getMouseLocation() {
	ImVec2 mousePosition = ImGui::GetIO().MousePos;
	ImVec2 regionSize = ImGui::GetWindowSize();
	ImVec2 cursorPosition = ImGui::GetCursorScreenPos();
    ImVec2 relativeMousePosition = ImVec2(mousePosition.x - cursorPosition.x, mousePosition.y - cursorPosition.y);

    float mouseX = clipValue(((float)relativeMousePosition.x - (regionSize.x / 2.0))/ (float) (regionSize.x / 2.0), -1.0, 1.0);
    float mouseY = -clipValue(((float)relativeMousePosition.y - (regionSize.y / 2.0))/ (float) (regionSize.y / 2.0), -1.0, 1.0);
	return glm::vec2{mouseX, mouseY};
}

void Viewport3D::cleanUp() {
    // Clean up all of the shaders used for rendering in this viewport:
    this->basicShader.cleanUpShader();
    this->lineShader.cleanUpShader();

	// Clean up the framebuffer we're rendering to
	this->surfaceFBO.cleanUp();
}

void Viewport3D::updateOrbitalCamera() {
    this->camera.rotation.x = -this->orbit.angle;
	this->camera.rotation.y = (M_PI / 2.0) + this->orbit.orientation;
	this->camera.rotation.z = 0.0f;
	
	float r = this->orbit.distance * cos(this->orbit.angle);

	this->camera.location.x = r * cos(this->orbit.orientation);
	this->camera.location.y = this->orbit.distance * sin(this->orbit.angle);
	this->camera.location.z = -1.0f * r * sin(this->orbit.orientation);

	this->camera.location.x += this->orbit.focusX;
	this->camera.location.y += this->orbit.focusY;
	this->camera.location.z += this->orbit.focusZ;
}
