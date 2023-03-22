#include "window.hpp"

Window::Window(const int width, const int height, const char* title) {
    // First, create the GLFW window and setup the GLFW environment.

    glfwSetErrorCallback(glfwErrorCallback);    

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        exit(EXIT_FAILURE);
    }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    this->glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);

    if (!this->glfwWindow) {
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

    // TODO: This is where to handle GLFW input callbacks.

    glfwMakeContextCurrent(this->glfwWindow);

    // Second, setup GLEW so that we can use modern OpenGL.

    GLenum error = glewInit();
	if (error != GLEW_OK) {
		std::cerr << "GLEW Error " << error << ", Description:\n" << glewGetErrorString(error) << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

    // Third, setup ImGui with the GLFW and OpenGL backends we're using.

    IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	this->imGuiIO = &ImGui::GetIO();
    
    // This setting is required so that viewports that implement dragging are not
    // overwritten by ImGui's docker draging.
    imGuiIO->ConfigWindowsMoveFromTitleBarOnly = true;

	imGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard |
        ImGuiConfigFlags_DockingEnable |
        ImGuiConfigFlags_ViewportsEnable;   // Enables multi-window docking.

	if (!ImGui_ImplGlfw_InitForOpenGL(this->glfwWindow, true)) {
		std::cout << "Failed ImGui Initialization (1)!" << std::endl;
        glfwTerminate();
		exit(EXIT_FAILURE);
	}

	if (!ImGui_ImplOpenGL3_Init()) {
		std::cout << "Failed ImGui Initialization (2)!" << std::endl;
        glfwTerminate();
		exit(EXIT_FAILURE);
	}

}

const Window::Dimension Window::getFramebufferSize() {
    Window::Dimension size;
    glfwGetFramebufferSize(this->glfwWindow, &size.width, &size.height);
    return size;
}

void Window::clearWindow() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::updateWindow() {
    // If the window was resized, update the viewport to match.
    Dimension size = this->getFramebufferSize();
    glViewport(0, 0, size.width, size.height);
    
    glfwSwapBuffers(this->glfwWindow);
    glfwPollEvents();
}

void Window::newImGuiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Window::renderImGui() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Handle the platform window rendering if necessary
    if (this->imGuiIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        
        // Update the window context to the main application after drawing
        // any external windows.
        glfwMakeContextCurrent(this->glfwWindow);
    }
}

void Window::restoreRenderTarget() {
    // Zero is the default window FBO target.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Update the viewport to fill the entire window.
    Dimension size = this->getFramebufferSize();
    glViewport(0, 0, size.width, size.height);
}

void Window::cleanUpWindow() {
    // Clean up everuthing ImGui
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    // Clean up everything GLFW
	glfwDestroyWindow(this->glfwWindow);
	glfwTerminate();
}

static void glfwErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ", Description:\n" << description << std::endl;
}