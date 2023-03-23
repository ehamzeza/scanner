#pragma once

#include <string>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "buffers.hpp"

class GLSurface {
private:
    std::string title;
    std::string childTitle;
    FBO fbo;

    // These two floats are in the OpenGL coordinate system.
    float mouseX, mouseY;
    // The ratio width / height.
    float aspectRatio;
        
    struct {float red, green, blue, alpha;} typedef Color;
    Color clearColor;

public:
    GLSurface();
    void createSurface(const std::string title, const int bufferWidth, const int bufferHeight);

    void clearSurface();
    void bindAsRenderTarget();

    void RenderImGui();

    void setClearColor(float red, float green, float blue, float alpha);
    const float getAspectRatio();
    const float getMouseX();
    const float getMouseY();

    void cleanUp();

private:
    void computeMouseCoordinates(const ImVec2 relativeMousePosition, const ImVec2 regionSize);
    float clipValue(float input, const float min, const float max);
};
