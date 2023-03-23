/**
 * @brief Windows implementation of the @class{SerialConnection} with influence 
 * from the following citations. Modified to islate windows functionality from 
 * a Serial connection interface.
 * @cite{1} https://stackoverflow.com/questions/2674048/what-is-proper-way-to-detect-all-available-serial-ports-on-windows
 * @cite{2} https://github.com/manashmandal/SerialPort
 */

#include <windows.h>

#include "serial.hpp"

static const int MAX_PATH_LENGTH = 512;
static const int MAX_PORT_NUMBER = 255;
static const int SETUP_WAIT_TIME = 1000;

static HANDLE handle;
static COMSTAT status;
static DWORD errors;

std::vector<DeviceListEntry> get_available_ports() {
    std::vector<DeviceListEntry> available_ports;

    // Allocate a buffer to store the port path in:
    char* path_buffer = new char[MAX_PATH_LENGTH];

    // Iterate over all the possible ports and query their availability:
    for (int i = 0; i <= MAX_PORT_NUMBER; i++) {
        std::string port_name = "COM" + std::to_string(i);

        DWORD result = QueryDosDevice(port_name.c_str(), path_buffer, MAX_PATH_LENGTH);

        // The port responded, add it to the list.
        if (result != 0) {
            available_ports.push_back(DeviceListEntry(port_name, port_name + " - " + std::string(path_buffer)));
        }
    }
    
    delete[] path_buffer;
    return available_ports;
}

SerialConnection::SerialConnection(const std::string port) {
    this->connected = false;

    handle = CreateFileA(static_cast<LPCSTR>(port.c_str()), GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (handle == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open port \"" << port << "\"" << std::endl;
        return;
    }

    // Attempt to get the current serial parameters
    DCB serial_parameters = {0};

    if (!GetCommState(handle, &serial_parameters)) {
        std::cerr << "Failed to read serial port parameters!" << std::endl;
        return;
    }

    // Setup specific additional parameters.
    serial_parameters.BaudRate = CBR_256000;
    serial_parameters.ByteSize = 8;
    serial_parameters.StopBits = ONESTOPBIT;
    serial_parameters.Parity = NOPARITY;
    serial_parameters.fDtrControl = DTR_CONTROL_ENABLE;
    
    if (!SetCommState(handle, &serial_parameters)) {
        std::cerr << "Could not set serial port parameters\n";
        return;
    }

    this->connected = true;
    PurgeComm(handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
    Sleep(SETUP_WAIT_TIME);
}

int SerialConnection::read_data(const char* buffer, unsigned int buffer_size) {
    DWORD num_read;
    unsigned int num_to_read = 0;

    ClearCommError(handle, &errors, &status);

    // If there are bytes to read, find out how many.
    if (status.cbInQue > 0) {
        num_to_read = status.cbInQue;

        // Make sure the buffer we're storing into is big enough
        if (num_to_read > buffer_size) {
            num_to_read = buffer_size;
        }
    }

    // Set the buffer to be zero-filled
    memset((void*) buffer, 0, buffer_size);

    // Attempt to read the bytes.
    if (ReadFile(handle, (void*) buffer, num_to_read, &num_read, NULL)) {
        return num_read;
    }

    // No bytes read, not necessarily unsuccessful.
    return 0;
}

bool SerialConnection::write_data(const char* buffer, unsigned int buffer_size) {
    DWORD bytes_sent;

    if (!WriteFile(handle, (void*) buffer, buffer_size, &bytes_sent, 0)) {
        ClearCommError(handle, &errors, &status);

        std::cerr << "Failed to send some data... (" << bytes_sent << "/" << buffer_size << ")!";
        return false;
    }

    return true;
}

SerialConnection::~SerialConnection() {
    this->close();
}

void SerialConnection::close() {
    if (this->connected) {
        CloseHandle(handle);
        this->connected = false;
    }
}