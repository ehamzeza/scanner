#include "usb_interface.hpp"

USBInterface::USBInterface(Application* application) {
    this->application = application;
}

void USBInterface::renderImGUI() {
    ImGui::Begin("Serial Connection Tools");

    ImGui::Text("Available Serial Ports:");
    
    ImGui::End();
}

void USBInterface::cleanUp() {

}
