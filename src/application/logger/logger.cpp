#include "logger.hpp"

void Logger::log(const std::string message) {
    this->logs.push_back(message);
}

void Logger::render_gui() {
    const std::string clear_button_text = "Clear Log";

    ImGui::Begin("Console Log Messages", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

    ImGui::Text(std::string("Log Messages:").c_str());

    // Determine the spacing to leave for the button on the bottom after this child.
    float spacing_leave = ImGui::CalcTextSize(clear_button_text.c_str()).y + (2.0 * ImGui::GetStyle().FramePadding.y);

    ImGui::BeginChild(1, ImVec2(-1.0f, ImGui::GetContentRegionMax().y - ImGui::GetCursorPosY() - spacing_leave), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    for (const std::string message : this->logs) {
        ImGui::Text(message.c_str());
    }
    ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    if (ImGui::Button("Clear Log")) {
        this->logs.clear();
    }

    ImGui::End();
}