#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>

#include "window.hpp"
#include "serial.hpp"
#include "logger.hpp"

static void renderApplicationGUI();

static std::vector<DeviceListEntry> device_list; 
static SerialConnection* connection;

static const unsigned int data_buffer_size = 256;
static char data_buffer[data_buffer_size];

std::queue<std::string> line_queue;
std::stringstream current_line_buffer;

static Logger* logger;

static int show_combo_box(std::vector<std::string> items, int* selected_index) {
    std::string current_item = items.at(*selected_index);

    if (ImGui::BeginCombo("##combo", current_item.c_str())) {
        
        for (int i = 0; i < items.size(); i++) {
            bool is_selected = current_item == items.at(i);

            if(ImGui::Selectable(items.at(i).c_str(), is_selected)) {
                current_item = items.at(i);
                *selected_index = i;
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    } 

    return *selected_index;
}

int main(const int argc, const char* argv[]) {
    Window* window = new Window(800, 600, "Scanner!");

    logger = new Logger();

    while (window->keepWindowAlive()) {

        if (connection != nullptr) {
            int r = connection->read_data(data_buffer, data_buffer_size);
            if (r > 0) {
                line_queue.push(std::string(data_buffer));
                // int buffer_traverse = 0;

                // while (data_buffer[buffer_traverse] != '\0') {
                //     if (data_buffer[buffer_traverse] == '\n') {
                //         line_queue.push(current_line_buffer.str());
                //         current_line_buffer = std::stringstream();
                //         break;
                //     }

                //     current_line_buffer << data_buffer[buffer_traverse];
                //     buffer_traverse ++;

                // }
            }
        }
        // std::cout << current_line_buffer.str() << std::endl;

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
    logger->render_gui();

    ImGui::Begin("Serial Connection Tools");

    ImGui::Text("Available Serial Ports:");

    static int serial_selected_index = 0;

    if (device_list.size() == 0) {
        std::vector<std::string> no_device_list = {"No Devices... Refresh"};
        show_combo_box(no_device_list, &serial_selected_index);
    } else {
        std::vector<std::string> device_descriptions;
        for (DeviceListEntry device : device_list) {
            device_descriptions.push_back(device.get_description());
        }
        show_combo_box(device_descriptions, &serial_selected_index);
    }

    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {
        logger->log("Refreshing device list...");
        device_list = get_available_ports();
    }

    ImGui::Text("Connection Controls:");

    if(ImGui::Button(std::string("Connect").c_str())) {
        std::string port_name = device_list.at(serial_selected_index).get_port();
        logger->log("Connecting to port \"" + port_name + "\"...");
        connection = new SerialConnection(port_name);
    }

    ImGui::SameLine();
    if(ImGui::Button(std::string("Disconnect").c_str())) {
        logger->log("Disconnecting...");
        if (connection != nullptr) {
            connection->close();
            delete connection;
        }
    }

    if (line_queue.size() > 0) {
        std::string line = line_queue.front();
        line_queue.pop();
        logger->log("Serial Message: \"" + line + "\"");
    }

    ImGui::End();
}
