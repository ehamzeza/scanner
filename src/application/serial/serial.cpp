#include "serial.hpp"

DeviceListEntry::DeviceListEntry(std::string port, std::string description) {
    this->port = port;
    this->description = description;
}

std::string DeviceListEntry::get_port() {
    return this->port;
}

std::string DeviceListEntry::get_description() {
    return this->description;
}

bool SerialConnection::is_connected() {
    return this->connected;
}