#include "usb_interface.hpp"

USBInterface::USBInterface(Application* application) {
    this->application = application;
    this->connection = nullptr;

    // Allocate the buffer for reading USB data into
    this->data_buffer = new char[this->data_buffer_size];
    for (int i = 0; i < this->data_buffer_size; i++)
        this->data_buffer[i] = '\0';

    this->command_input_buffer = new char[this->command_input_buffer_size];
    for (int i = 0; i < this->command_input_buffer_size; i++)
        this->command_input_buffer[i] = '\0';
    
    this->command_output_buffer = new char[this->command_output_buffer_size];
    for (int i = 0; i < this->command_output_buffer_size; i++)
        this->command_output_buffer[i] = '\0';

    this->serial_selected_index = 0;
}

void USBInterface::updateRead() {
    if (this->connection == nullptr)
        return;

    if (!this->connection->is_connected())
        return;

    int num_chars_read = this->connection->read_data(this->data_buffer, this->data_buffer_size);
    if (num_chars_read <= 0)
        return;

    for (int i = 0; i < num_chars_read; i++)
        this->captured_serial_data.push_back(this->data_buffer[i]);
}

void USBInterface::processSerialIO() {
    // If there is a message in the scan queue that is ready and we are currently
    // in a scanning mode, send it.
    if (this->scan_data.scan_started && this->scan_data.command_queue.size() > 0) {
        ScanCommand& next = this->scan_data.command_queue.front();

        if (!next.sent) {
            next.sent = true;

            const int send_buffer_size = 32;
            char* send_buffer = new char[send_buffer_size];

            for (int i = 0; i < send_buffer_size; i++)
                send_buffer[i] = 0;

            int message_size = this->messageFromString(next.command, send_buffer, send_buffer_size);

            if (message_size > 0) {
                this->application->logger->log("Sending Scan Command \"" + next.command + "\"...");
                this->connection->write_data(send_buffer, message_size);
            } else {
                this->application->logger->log("Invalid Scan Command \"" + next.command + "\"!");
            }

            delete[] send_buffer;
        }
    }

    // If there aren't any incoming messages, don't continue.
    int size = this->captured_serial_data.size();
    if (size <= 0)
        return;

    // Process incoming messages if they exist.
    switch (this->captured_serial_data.front()) {
        case 'r':
            if (size >= 5) {
                this->captured_serial_data.pop_front();
                int readyCode = this->readSerialInt();

                this->processCommandR(readyCode);
            }
            break;
        
        case 'z':
            if (size >= 2) {
                this->captured_serial_data.pop_front();
                char axis = this->captured_serial_data.front();
                this->captured_serial_data.pop_front();
                this->processCommandZ(axis);
            }
            break;
        
        case 'h':
            if (size >= 5) {
                this->captured_serial_data.pop_front();
                float heightValue = this->readSerialFloat();
                this->processCommandH(heightValue);
            }
            break;

        case 'g':
            if (size >= 1) {
                this->captured_serial_data.pop_front();
                this->processCommandG();
            }
            break;

        case 'd':
            if (size >= 9) {
                this->captured_serial_data.pop_front();
                int index = this->readSerialInt();
                float distance = this->readSerialFloat();

                this->processCommandD(index, distance);
            }
            break;
        
        case 's':
            if (size >= 1) {
                this->captured_serial_data.pop_front();
                this->processCommandS();
            }
            break;

        case 'q':
            if (size >= 1) {
                this->captured_serial_data.pop_front();
                this->processCommandQ();
            }
        
        case '$':
            if (size >= 5) {
                // We need to determine if the entirety of the string has arrived before
                // popping anything from the buffer, determine the size here.
                union {
                    int num;
                    char bytes[4];
                } data;

                std::vector<char> vec(this->captured_serial_data.begin(), this->captured_serial_data.end());
                for (int i = 0; i < 4; i++) {
                    data.bytes[i] = vec.at(i + 1); // + 1 offset for message character.
                }

                // Do we have enough information for whole string?
                if (size >= 5 + data.num) {
                    // Pop the message character and integer size segment.
                    for (int i = 0; i < 5; i++)
                        this->captured_serial_data.pop_front();
                    
                    std::stringstream stream;
                    for (int i = 0; i < data.num; i++) {
                        stream << this->captured_serial_data.front();
                        this->captured_serial_data.pop_front();
                    }

                    this->application->logger->log("USB >> \"" + stream.str() + "\"");
                }
            }
            break;
        
        default:
            std::string unknown = "";
            unknown += this->captured_serial_data.front();
            
            if (unknown == "\n")
                unknown = "\\n";
            
            this->application->logger->log(std::string("Unexpected char '") + unknown + std::string("'!"));
            this->captured_serial_data.pop_front();
            break;
    }
}

void USBInterface::startScan() {
    if (this->scan_data.scan_started) {
        this->resetScan();
    }

    ScanCommand ready {
        .command = "r"
    };
    this->scan_data.command_queue.push(ready);

    this->scan_data.scan_started = true;
}

void USBInterface::resetScan() {
    this->application->logger->log("Clearing existing scan data...");

    this->scan_data.scan_started = false;
    this->scan_data.scanned_points.clear();
    
    while (this->scan_data.command_queue.size() > 0) {
        this->scan_data.command_queue.pop();
    }
}

void USBInterface::processCommandR(int code) {
    if (!this->scan_data.scan_started) {
        return;
    }

    if (code > 0) {
        this->scan_data.command_queue.pop();

        ScanCommand zeroR {
            .command = "z r"
        };
        this->scan_data.command_queue.push(zeroR);

        ScanCommand zeroZ {
            .command = "z z"
        };
        this->scan_data.command_queue.push(zeroZ);

        ScanCommand height {
            .command = "h"
        };
        this->scan_data.command_queue.push(height);
        
    } else {
        this->application->logger->log("ERROR: Unexpected r command");
    }
}

void USBInterface::processCommandZ(char axis) {
    if (this->scan_data.command_queue.front().command.find("z") == 0 &&
        (axis == 'r' || axis == 'z')) {
        this->scan_data.command_queue.pop();
    } else {
        this->application->logger->log("ERROR: Unexpected z command");
    }
}

void USBInterface::processCommandH(float height) {
    this->application->logger->log("Calculating scan commands...");
    this->scan_data.command_queue.pop();

    int num_rows = height / this->scan_data.scan_z_resolution;

    for (int i = 0; i < num_rows; i++) {
        // Move to row.
        ScanCommand moveUp {
            .command = "g z " + std::to_string(this->scan_data.scan_z_resolution * i)
        };
        this->scan_data.command_queue.push(moveUp);

        // Move R to zero
        // ScanCommand moveRZero {
        //     .command = "g r 0.0"
        // };
        // this->scan_data.command_queue.push(moveRZero);

        // Scan this row
        ScanCommand scanRow;
        scanRow.command = "s " + std::to_string((int) (360.0f / this->scan_data.scan_arc_resolution)) + " " + std::to_string(this->scan_data.scan_arc_resolution);
        std::cout << "command scanrow" << scanRow.command << std::endl;
        scanRow.row_height = this->scan_data.scan_z_resolution * i;
        scanRow.arc_offset = 0.0f;
        this->scan_data.command_queue.push(scanRow);
    }
}

void USBInterface::processCommandD(int index, float distance) {
    // Check to see that the curren't command is an S command
    if (!this->scan_data.command_queue.front().command.find("s") == 0) {
        this->application->logger->log("ERROR: Recieved data packet outside of scan command!");
        return;
    }

    float angle = (this->scan_data.scan_arc_resolution * index) * (M_PI / 180.0f);
    angle += this->scan_data.command_queue.front().arc_offset;

    float x = 0.1f * distance * cos(angle);
    float y = 0.1f * this->scan_data.command_queue.front().row_height;
    float z = 0.1f * distance * sin(angle);

    this->scan_data.scanned_points.push_back(glm::vec3(x, y, z));
}

void USBInterface::processCommandS() {
    this->scan_data.command_queue.pop();
}

void USBInterface::processCommandG() {
    if (this->scan_data.scan_started) {
        this->scan_data.command_queue.pop();
    }
}

void USBInterface::processCommandQ() {
    this->application->logger->log("ERROR: Stop requested from scanner!");
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
    if (ImGui::Button(std::string("Disconnect").c_str())) {
        this->application->logger->log("Disconnecting...");
        if (this->connection != nullptr) {
            this->connection->close_connection();
            delete this->connection;
            this->connection = nullptr;
        }
    }

    ImGui::Separator();
    ImGui::Text("Manual Communication Controls:");

    ImGui::InputText(std::string("Command: ").c_str(), this->command_input_buffer, this->command_input_buffer_size);

    ImGui::SameLine();
    if (ImGui::Button(std::string("Send!").c_str())) {
        if (this->scan_data.scan_started) {
            this->application->logger->log("WARNING: Sending manual commands with active scan, reseting scan...");
            this->resetScan();
        }

        std::string command(this->command_input_buffer);
        int message_size = this->messageFromString(command, this->command_output_buffer, this->command_output_buffer_size);

        if (message_size > 0) {
            this->application->logger->log("Sending \"" + command + "\"...");
            this->connection->write_data(this->command_output_buffer, message_size);
        } else {
            this->application->logger->log("Invalid message \"" + command + "\"!");
        }
    }

    if (ImGui::Button(std::string("Reset Input").c_str())) {
        this->captured_serial_data.clear();
    }

    ImGui::Separator();
    ImGui::Text("Scan Controls:");

    ImGui::InputFloat("Z-Resolution (mm)", &this->scan_data.scan_z_resolution, 1.0f);
    ImGui::InputFloat("R-Resolution (deg)", &this->scan_data.scan_arc_resolution, 1.0f);

    if (ImGui::Button(std::string("Start Scan").c_str())) {
        this->startScan();
    }

    ImGui::SameLine();
    if (ImGui::Button(std::string("Reset Scan").c_str())) {
        this->resetScan();
    }

    if (!already_connected) {
        ImGui::EndDisabled();
    }

    ImGui::End();
}

void USBInterface::cleanUp() {
    delete[] this->data_buffer;
    delete[] this->command_input_buffer;
    delete[] this->command_output_buffer;

    // Close the connection if it exists
    if (this->connection != nullptr) {
        this->connection->close_connection();
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

int USBInterface::readSerialInt() {
    if (this->captured_serial_data.size() < 4) {
        this->application->logger->log("Attempted to read read serial int32 without enough bytes!");
        return 0;
    }

    union {
        char bytes[4];
        int value;
    } data;

    for (int i = 0; i < 4; i++) {
        data.bytes[i] = this->captured_serial_data.front();
        this->captured_serial_data.pop_front();
    }

    return data.value;
}

float USBInterface::readSerialFloat() {
    if (this->captured_serial_data.size() < 4) {
        this->application->logger->log("Attempted to read read serial float without enough bytes!");
        return 0;
    }

    union {
        char bytes[4];
        float value;
    } data;

    for (int i = 0; i < 4; i++) {
        data.bytes[i] = this->captured_serial_data.front();
        this->captured_serial_data.pop_front();
    }

    return data.value;
}

int USBInterface::messageFromString(std::string message, char* out_buffer, int out_buffer_size) {
    // No message is shorter than one character
    if (message.length() < 1) return -1;

    // Make sure the output buffer is big enough for the longest message
    if (out_buffer_size < 16) {
        std::cerr << "Output message buffer not big enough!" << std::endl;
        return -1;
    }

    // Stream the message based on whitespace and produce the byte representation
    std::stringstream stream(message);
    
    char control_char;
    stream >> control_char;

    char buffer_char;
    int buffer_int;
    float buffer_float;

    switch (control_char) {
        case 'r':
            this->captured_serial_data.clear();
            out_buffer[0] = 'r';
            return 1;

        case 'z':
            out_buffer[0] = 'z';
            stream >> buffer_char;
            out_buffer[1] = buffer_char;
            return 2;
        
        case 'h':
            out_buffer[0] = 'h';
            return 1;

        case 'g':
            out_buffer[0] = 'g';
            stream >> buffer_char;
            out_buffer[1] = buffer_char;
            stream >> buffer_float;
            memcpy(out_buffer + 2, &buffer_float, sizeof(float));
            return 6;
        
        case 's':
            out_buffer[0] = 's';
            stream >> buffer_int;
            memcpy(out_buffer + 1, &buffer_int, sizeof(int));
            stream >> buffer_float;
            memcpy(out_buffer + 5, &buffer_float, sizeof(float));
            return 9;
    };
}
