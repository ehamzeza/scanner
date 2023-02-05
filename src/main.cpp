#include <iostream>
#include <string>

#include <libusb-1.0/libusb.h>

static void print_connected_devices();

int main(const int argc, const char* argv[]) {
    print_connected_devices();
    return 0;
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
