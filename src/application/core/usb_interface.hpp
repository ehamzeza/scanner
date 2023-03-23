#pragma once

#include "application.hpp"
#include "serial.hpp"

class Application;

class USBInterface {
private:
    Application* application;

    std::vector<DeviceListEntry> device_list; 
    SerialConnection* connection;
    int serial_selected_index;

public:
    USBInterface(Application* application);
    void renderImGUI();
    void cleanUp();

private:
    int show_combo_box(std::vector<std::string> items, int* selected_index);
};