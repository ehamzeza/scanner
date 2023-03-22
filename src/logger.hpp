#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

class Logger {
private:
    std::vector<std::string> logs;
public:
    Logger() {};
    void log(const std::string message);
    void render_gui();
};