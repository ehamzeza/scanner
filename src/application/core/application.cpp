#include "application.hpp"

Application::Application() {
	this->runApplication = true;
	this->exitStatus = 0;

	this->window = nullptr;
}

const int Application::startApplication(const int width, const int height) {
	this->window = new Window(width, height, "3D Scanner Sofware!");

	this->coordVAO = this->loader.createCoordinate(1.0f);
	this->cubeVAO = this->loader.createFromOBJ("../res/cube.obj");
	this->gridVAO = this->loader.createGridXZ(10.0, 10.0, 1.0);
	
	// this->slicingModel = Model(this->cubeVAO);
	// this->slicingModel.location = glm::vec3{0.0, 1.0, 0.0};

	this->viewport.createViewport("3D Viewport", width, height);
	// this->graph.createGraph("2D Graph", width, height);

	while (this->runApplication) {
		/*********************************
		 * Render the 3D Viewport Surface:
		 *********************************/
		this->viewport.bindAsRenderTarget();
		this->viewport.updateViewport();
		this->viewport.clearViewport();

		// Render the Central Coordinate System:
		Model coordModelLarge(this->coordVAO);
		coordModelLarge.setScale(4.0);
		this->viewport.renderLines(coordModelLarge, 3.0f);

		// Render the Main Grid of the Viewport:
		Model gridModel(this->gridVAO);
		this->viewport.renderLines(gridModel, 1.0);

		// Render the Model That We Are "Slicing":
		// this->viewport.renderModel(this->slicingModel, {1.0, 0.0, 0.0});

		// Render the Focal Point of the Viewport Camera:
		Model focalCubeModel(this->cubeVAO);
		focalCubeModel.location = {this->viewport.orbit.focusX, this->viewport.orbit.focusY, this->viewport.orbit.focusZ};
		focalCubeModel.rotation = {0.0, 0.0, 0.0};
		focalCubeModel.scale = {0.1, 0.1, 0.1};
		this->viewport.renderModel(focalCubeModel, {1.0, 1.0, 0.0});

		/********************************************************************
		 * Before rendering anything ImGui, restore the default render target
		 ********************************************************************/
		this->window->restoreRenderTarget();
		this->window->clearWindow();

		// Render anything related to ImGui
		this->renderApplicationGUI();

		// Finally, update the frame being drawn and see if the window should exit.
		this->window->updateWindow();
		this->runApplication &= this->window->keepWindowAlive();
	}

	this->cleanUpAplication();
	return this->exitStatus;
}

void Application::cleanUpAplication() {
	this->coordVAO.cleanUp();
	this->cubeVAO.cleanUp();
	this->gridVAO.cleanUp();

	this->viewport.cleanUp();
	// this->graph.cleanUp();

	this->window->cleanUpWindow();
	delete this->window;
}

void Application::renderApplicationGUI() {
	this->window->newImGuiFrame();

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	this->viewport.RenderImGui();
	// this->graph.RenderImGui();

	// Render the model positioning ImGui
	// ImGui::Begin("Model Control");
	// ImGui::SliderFloat("Origin X", &this->slicingModel.location.x, -4, 4, "%.3f", 0);
	// ImGui::SliderFloat("Origin Y", &this->slicingModel.location.y, -4, 4, "%.3f", 0);
	// ImGui::SliderFloat("Origin Z", &this->slicingModel.location.z, -4, 4, "%.3f", 0);
	// ImGui::SliderFloat("Rotation X", &this->slicingModel.rotation.x, -4, 4, "%.3f", 0);
	// ImGui::SliderFloat("Rotation Y", &this->slicingModel.rotation.y, -4, 4, "%.3f", 0);
	// ImGui::SliderFloat("Rotation Z", &this->slicingModel.rotation.z, -4, 4, "%.3f", 0);
	// ImGui::SliderFloat("Scale X", &this->slicingModel.scale.x, -4, 4, "%.3f", 0);
	// ImGui::SliderFloat("Scale Y", &this->slicingModel.scale.y, -4, 4, "%.3f", 0);
	// ImGui::SliderFloat("Scale Z", &this->slicingModel.scale.z, -4, 4, "%.3f", 0);
	// ImGui::End();

	ImGui::ShowDemoWindow();

	this->window->renderImGui();
}

const void Application::terminateApplication(const int exitStatus) {
	this->exitStatus = exitStatus;
	this->runApplication = false;
}
