#include "usb_interface.hpp"

USBInterface::USBInterface(Application* application) {
    this->application = application;

    this->serial_selected_index = 0;
}

void USBInterface::renderImGUI() {
    ImGui::Begin("Serial Connection Tools");

    ImGui::Text("Available Serial Ports:");
    
    bool already_connected = (this->connection != nullptr) && this->connection->is_connected();

    if (already_connected) {
        ImGui::BeginDisabled();
    }

    if (this->device_list.size() == 0) {
        std::vector<std::string> no_device_list = {"No Devices... Refresh"};
        show_combo_box(no_device_list, &serial_selected_index);
    } else {
        std::vector<std::string> device_descriptions;
        for (DeviceListEntry device : this->device_list) {
            device_descriptions.push_back(device.get_description());
        }
        show_combo_box(device_descriptions, &serial_selected_index);
    }

    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {
        this->application->logger->log("Refreshing device list...");
        this->device_list = get_available_ports();
    }

    ImGui::Text("Connection Controls:");

    if (ImGui::Button(std::string("Connect").c_str())) {
        std::string port_name = this->device_list.at(this->serial_selected_index).get_port();
        this->application->logger->log("Connecting to port \"" + port_name + "\"...");
        this->connection = new SerialConnection(port_name);

        // Check to see if the status of the connection and notify the user
        if(!this->connection->is_connected()) {
            this->application->logger->log("Failed to connect to port \"" + port_name + "\"!");
        } else {
            this->application->logger->log("Port \"" + port_name + "\" connected!");
        }
    }

    if (already_connected) {
        ImGui::EndDisabled();
    } else {
        ImGui::BeginDisabled();
    }

    ImGui::SameLine();
    if(ImGui::Button(std::string("Disconnect").c_str())) {
        this->application->logger->log("Disconnecting...");
        if (this->connection != nullptr) {
            this->connection->close();
            delete this->connection;
        }
    }

    if(!already_connected) {
        ImGui::EndDisabled();
    }

    ImGui::End();
}

void USBInterface::cleanUp() {
    // Close the connection if it exists
    if (this->connection != nullptr) {
        this->connection->close();
        delete this->connection;
    }
}

int USBInterface::show_combo_box(std::vector<std::string> items, int* selected_index) {
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
