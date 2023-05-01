#pragma once

#include "application.hpp"
#include "serial.hpp"
#include "scan_data.hpp"
#include "obj_writer.hpp"
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

    const unsigned int file_output_buffer_size = 1024;
    char* file_output_buffer;

    const unsigned int command_output_buffer_size = 32;
    char* command_output_buffer;

public:
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
    void processCommandG();
    void processCommandD(int index, float distance);
    void processCommandS();
    void processCommandQ(); // Error command

    int readSerialInt();
    float readSerialFloat();

private:
    int messageFromString(std::string message, char* out_buffer, int out_buffer_size);
    int show_combo_box(std::vector<std::string> items, int* selected_index);
};