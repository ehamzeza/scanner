#include <iostream>
#include <vector>
#include <string>

#include "window.hpp"
#include "serial.hpp"

static void renderApplicationGUI();

static std::vector<std::string> device_list; 
static SerialConnection* connection;

static const unsigned int data_buffer_size = 256;
static char data_buffer[data_buffer_size];

int main(const int argc, const char* argv[]) {
    Window* window = new Window(800, 600, "Scanner!");

    while (window->keepWindowAlive()) {

        if (connection != nullptr) {
            int r = connection->read_data(data_buffer, data_buffer_size);
            if (r > 0)
                std::cout << "DATA: " << r << ":" << data_buffer << std::endl;
        }

        /********************************************************************
		 * Before rendering anything ImGui, restore the default render target
		 ********************************************************************/
		window->restoreRenderTarget();
		window->clearWindow();

		// Render anything related to ImGui
        window->newImGuiFrame();
		renderApplicationGUI();
        window->renderImGui();

		// Finally, update the frame being drawn and see if the window should exit.
		window->updateWindow();
    }

    std::cout << "Cleaning up..." << std::endl;
    window->cleanUpWindow();
    delete window;

    if (connection != nullptr) {
        connection->close();
        delete connection;
    }
    exit(EXIT_SUCCESS);
}

void renderApplicationGUI() {
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	ImGui::ShowDemoWindow();

    ImGui::Begin("Serial Connection Tools");
    if (ImGui::Button("List Ports")) {
        std::cout << "Listing Ports..." << std::endl;
        device_list = get_available_ports();
    }

    for(std::string item : device_list) {
        ImGui::Text(item.c_str());
    }

    if (ImGui::Button("Connect")) {
        std::cout << "Connected!" << std::endl;
        connection = new SerialConnection("COM5");
    }

    if (ImGui::Button("Disconnect")) {
        std::cout << "Disconnecting..." << std::endl;
        if (connection != nullptr) {
            connection->close();
        }
    }

    ImGui::End();
}
