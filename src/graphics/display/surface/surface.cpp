#include "surface.hpp"

GLSurface::GLSurface() {
    
}

void GLSurface::createSurface(const std::string title, const int bufferWidth, const int bufferHeight) {
    this->title = title;
    this->childTitle = title + "_Child";
    this->aspectRatio = 1.0f;
    this->clearColor = {0.0, 0.0, 0.0, 1.0};

    // Create the FBO backend for rendering.
    this->fbo.create(bufferWidth, bufferHeight);
    this->fbo.addDepthStencilAttachment();
    this->fbo.addColorTextureAttachment();
}

void GLSurface::clearSurface() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(this->clearColor.red, this->clearColor.green, this->clearColor.blue, this->clearColor.alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLSurface::bindAsRenderTarget() {
    // Bind the FBO backend as the render target
    this->fbo.bind();

    // Resize the viewport to match the virtual surface size.
    glViewport(0, 0, this->fbo.getWidth(), this->fbo.getHeight());
}

void GLSurface::RenderImGui() {
    ImGui::Begin(this->title.c_str());

    ImGui::BeginChild(this->childTitle.c_str());

    ImVec2 windowSize = ImGui::GetWindowSize();
    this->aspectRatio = (float) windowSize.x / (float) windowSize.y;

	ImVec2 cursorPosition = ImGui::GetCursorScreenPos();
	ImVec2 mousePosition = ImGui::GetIO().MousePos;
    ImVec2 relativeMousePosition = ImVec2(mousePosition.x - cursorPosition.x, mousePosition.y - cursorPosition.y);
    this->computeMouseCoordinates(relativeMousePosition, windowSize);

    // Dragging tests
    bool active = ImGui::IsItemActive();
    ImVec2 delta = ImGui::GetMouseDragDelta();
    std::cout << "delta: " << delta.x << ", " << delta.y << "\t active: " << active << std::endl;

	// OpenGL's texture origin is different than ImGui's, it needs to be inverted.
    ImGui::Image((ImTextureID)this->fbo.getColorTextureId(), windowSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
    ImGui::EndChild();

    ImGui::End();
}

void GLSurface::setClearColor(float red, float green, float blue, float alpha) {
    this->clearColor.red = red;
    this->clearColor.green = green;
    this->clearColor.blue = blue;
    this->clearColor.alpha = alpha;
}

const float GLSurface::getAspectRatio() {
    return this->aspectRatio;
}

const float GLSurface::getMouseX(){
    return this->mouseX;
}
const float GLSurface::getMouseY(){
    return this->mouseY;
}

void GLSurface::cleanUp() {
    this->fbo.cleanUp();
}

void GLSurface::computeMouseCoordinates(const ImVec2 relativeMousePosition, const ImVec2 regionSize) {
    this->mouseX = clipValue(((float)relativeMousePosition.x - (regionSize.x / 2.0))/ (float) (regionSize.x / 2.0), -1.0, 1.0);
    this->mouseY = -clipValue(((float)relativeMousePosition.y - (regionSize.y / 2.0))/ (float) (regionSize.y / 2.0), -1.0, 1.0);
}

float GLSurface::clipValue(float input, const float min, const float max) {
    input = input > max ? max : input;
    input = input < min ? min : input;
    return input;
}
