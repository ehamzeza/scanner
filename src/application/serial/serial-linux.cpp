#include "serial.hpp"

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

std::vector<DeviceListEntry> get_available_ports() {
	std::vector<DeviceListEntry> entries;

	entries.push_back(DeviceListEntry("/dev/ttyACM0", "Arduino (HC): /dev/ttyACM0"));

	return entries;
}

static int serial_fd;

SerialConnection::SerialConnection(const std::string port) {
	this->connected = false;

	// Attempt to open the file for reading and writing.
	serial_fd = open(port.c_str(), O_RDWR | O_NONBLOCK);
	if (serial_fd < 0) {
		std::cerr << "Failed to open port \"" << port << "\" for reading!" << std::endl;
		return;
	}

	// Attempt to get the port options to set the BAUD rate
	termios toptions;
	if (tcgetattr(serial_fd, &toptions) < 0) {
        std::cerr << "Failed to get port term options!" << std::endl;
        return;
    }
	cfsetispeed(&toptions, B9600);
	cfsetospeed(&toptions, B9600);

	toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    //toptions.c_cflag &= ~HUPCL; // disable hang-up-on-close to avoid reset

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 0;
    //toptions.c_cc[VTIME] = 20;
    
    tcsetattr(serial_fd, TCSANOW, &toptions);
    if( tcsetattr(serial_fd, TCSAFLUSH, &toptions) < 0) {
        std::cerr << "Failed to set term attributes" << std::endl;
        return;
    }

	this->connected = true;
}

int SerialConnection::read_data(char* buffer, unsigned int buffer_size) {
	
	for (unsigned int i = 0; i < buffer_size; i++)
		buffer[i] = 0;

	int num_read = read(serial_fd, (void*) buffer, buffer_size);

	if (num_read == -1) {
		std::cout << "error reading" << std::endl;
	}

	return num_read;
}

bool SerialConnection::write_data(const char* buffer, unsigned int buffer_size) {
	int num_written = write(serial_fd, buffer, buffer_size);

	if (num_written < 0 || num_written != buffer_size) {
		std::cout << "Failed to write all bytes! Continuing..." << std::endl;
		return false;
	}
	return true;
}

SerialConnection::~SerialConnection() {
	this->close_connection();
}

void SerialConnection::close_connection() {
	if (this->connected) {
		close(serial_fd);
		this->connected = false;
	}
}



// TODO Incorporate this file to provide the linux-version of the serial.h header implementation.

/*
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
*/