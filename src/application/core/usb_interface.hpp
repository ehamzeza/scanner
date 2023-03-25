#pragma once

#include "application.hpp"
#include "serial.hpp"

#include <glm/vec3.hpp>

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
    std::string command;
    bool sent = false;
} typedef ScanCommand;

struct {
    bool scan_started = false;
    float scan_height = 0.0f; // milimeters
    float scan_z_resolution = 10.0f; // milimeters
    float scan_arc_resolution = 36.0f; // degrees
    std::vector<glm::vec3> scanned_points;
    std::queue<ScanCommand> command_queue;
} typedef ScanData;
ScanData scan_data;

public:
    USBInterface(Application* application);
    void updateRead();
    void processSerialIO();
    void renderImGUI();
    void cleanUp();

protected:
    void startScan();
    void resetScan();

    void processCommandR(int code);
    void processCommandZ(char axis);
    void processCommandH(float height);

    int readSerialInt();
    float readSerialFloat();

private:
    int messageFromString(std::string message, char* out_buffer, int out_buffer_size);
    int show_combo_box(std::vector<std::string> items, int* selected_index);
};