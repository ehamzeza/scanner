#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

static void glfwErrorCallback(int error, const char *description);

class Window {
public:
    struct Dimension {
        int width, height;
    };

private:
    GLFWwindow *glfwWindow;
    ImGuiIO *imGuiIO;

public:
    /**
     * @brief Creates and shows a new GLFW window. Additionally, it sets up the GLEW
     * extension loader and ImGui. This window must be cleaned up with the 
     *  cleanUpWindow method.
     *
     * @param width The width of the contents of the GLFW window.
     * @param height The height of the contents of the GLFW window.
     * @param title Title of the displayed window.
     */
    Window(const int width, const int height, const char *title);

    /**
     * @brief Returns the size in pixels of the window framebuffer. This is useful
     * for glViewport() and related commands.
     * 
     * @return Dimension Struct containing the mentioned information.
     */
    const Dimension getFramebufferSize();

    /**
     * @brief Returns true if the window should remain alive. (I.e. the user hasn't
     * pressed the close button)
     * 
     * @return true If the window should remain alive.
     * @return false If the application should clean up and close.
     */
    inline bool keepWindowAlive() {
        return !glfwWindowShouldClose(this->glfwWindow);
    }

    /**
     * @brief Clears the window to the default color in preparation for another
     * render cycle. Note: The window's FBO context must be current, or it will
     * clear the contents of the FBO buffer instead. This can be done with the
     * restoreRenderTarget() function. 
     */
    void clearWindow();

    /**
     * @brief Handles the required GLFW commands to update the contents of the
     * window. 
     */
    void updateWindow();

    /**
     * @brief Creates a new ImGui frame for rendering. This is using the OpenGL3
     * and GLFW backends. 
     */
    void newImGuiFrame();

    /**
     * @brief Draws the ImGui frame onto the window using the OpenGL3 and GLFW
     * backends.
     */
    void renderImGui();

    /**
     * @brief When using FBO objects as alternative render targets, this function
     * needs to be called before attempting to draw to this window again. 
     */
    void restoreRenderTarget();

    /**
     * @brief Call this method to handle cleaning up and closing the GLFW window, as well
     * as ImGui. This method must be called whenever a window has been created by the application. 
     */
    void cleanUpWindow();
};
