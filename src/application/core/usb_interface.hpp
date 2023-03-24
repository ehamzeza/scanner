#pragma once

#include "application.hpp"
#include "serial.hpp"

#include <sstream>
#include <string>
#include <queue>
#include <cmath>
#include <list>

class Application;

class USBInterface {
private:
    Application* application;

    std::vector<DeviceListEntry> device_list; 
    SerialConnection* connection;
    int serial_selected_index;

    std::list<char> captured_serial_data;

    const unsigned int data_buffer_size = 256;
    char* data_buffer;

    const unsigned int command_input_buffer_size = 32;
    char* command_input_buffer;

    const unsigned int command_output_buffer_size = 32;
    char* command_output_buffer;

struct {
    bool scan_started = false;
    float scan_height = 0.0f; // milimeters
    float scan_z_resolution = 10.0f; // milimeters
    float scan_arc_resolution = 36.0f; // degrees
} typedef ScanData;
ScanData scan_data;

public:
    USBInterface(Application* application);
    void updateRead();
    void processInput();
    void renderImGUI();
    void cleanUp();

private:
    int messageFromString(std::string message, char* out_buffer, int out_buffer_size);
    int show_combo_box(std::vector<std::string> items, int* selected_index);
};