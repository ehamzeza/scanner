#pragma once

#include "application.hpp"

class Application;

class USBInterface {
private:
    Application* application;

public:
    USBInterface(Application* application);
    void renderImGUI();
    void cleanUp();
};