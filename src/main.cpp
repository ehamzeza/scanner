#include <iostream>
#include <string>

#include <libusb-1.0/libusb.h>

#include "window.hpp"

static void print_connected_devices();
void renderApplicationGUI();

int main(const int argc, const char* argv[]) {
    Window* window = new Window(800, 600, "Scanner!");

    while (window->keepWindowAlive()) {

        /********************************************************************
		 * Before rendering anything ImGui, restore the default render target
		 ********************************************************************/
		window->restoreRenderTarget();
		window->clearWindow();

		// Render anything related to ImGui
        window->newImGuiFrame();
		renderApplicationGUI();
        window->renderImGui();

		// Finally, update the frame being drawn and see if the window should exit.
		window->updateWindow();
    }

    window->cleanUpWindow();
    delete window;

    exit(EXIT_SUCCESS);
}

void renderApplicationGUI() {
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	ImGui::ShowDemoWindow();
}

static void print_connected_devices() {
    // For single-peripheral communication, default null context is acceptable.
    struct libusb_context* usb_context = nullptr;

    // Initialize LibUSB, check for errors:
    int rc = libusb_init(&usb_context);
    if (rc != LIBUSB_SUCCESS) {
        std::cerr << "Failed to initialize LibUSB: RC = " << rc << std::endl;
        return;
    }

    // Fetch the device list:
    libusb_device** usb_devices;
    ssize_t num_connected_devices = libusb_get_device_list(usb_context, &usb_devices);

    // Output device information:
    std::cout << "Connected USB Devices: " << num_connected_devices << std::endl;
    for (int i = 0; i < num_connected_devices; i++) {
        int bus = libusb_get_bus_number(usb_devices[i]);
        int port = libusb_get_port_number(usb_devices[i]);
        
        std::cout << "DEVICE INDEX: " << i << "\tBUS: " << bus << "\t PORT: " << port << std::endl;
    }

    // Clean up and terminate LibUSB:
    libusb_free_device_list(usb_devices, 1);
    libusb_exit(usb_context);
}
