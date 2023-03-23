#pragma once

#include <memory>

#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>

#include "basic_shader.hpp"
#include "line_shader.hpp"
#include "camera.hpp"
#include "mouse.hpp"
#include "model.hpp"
#include "math.hpp"

class Viewport3D {
private:
    MousePicker mousePicker;
    Camera camera;

    BasicShader basicShader;
    LineShader lineShader;

    std::string title;

    /**
     * All of the variables used for rendering the OpenGL "surface".
     */
    FBO surfaceFBO;
    float surfaceAspectRatio;
    std::shared_ptr<glm::mat4> surfaceProjectionMatrix;
    std::shared_ptr<glm::mat4> surfaceViewMatrix;

public:
	struct OrbitData {
		float distance = 0.0f;
		float focusX = 0.0f, focusY = 0.0f, focusZ = 0.0f;
		float orientation = 0.0f, angle = 0.0f;
	};
    OrbitData orbit;

    // This is a snapshot of the orbit when the mouse is first pressed.
    OrbitData mouseDownOrbit;
    glm::vec3 mouseDownCameraLocation, mouseDownIntersection, mouseDownFarPlane;
    const float farPlaneDistance = 1000.0f;
public:
    Viewport3D();
    void createViewport(const std::string title, const int renderWidth, const int renderHeight);

    // This function needs to be called so that the projection and view matrices
    // can be recalculated.
    void updateViewport();

    void bindAsRenderTarget();
    void clearViewport();

    void renderLines(Model& model, const float lineWidth);
    void renderModel(Model& model, const glm::vec3 color);

    // The panning is implemented similar to https://prideout.net/blog/perfect_panning/.
    void handleInput();
    void RenderImGui();

    glm::vec2 getMouseLocation();

    void cleanUp();

private:
    void updateOrbitalCamera();
};
