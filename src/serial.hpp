#pragma once

#include <iostream>
#include <vector>
#include <string>

/**
 * @brief Get the list of available ports on the system as a vector
 * of port names represented as strings.
 * 
 * @return std::vector<std::string> 
 */
std::vector<std::string> get_available_ports();

/**
 * @brief Class that represents a general serial connection and the
 * ability to send / recieve bytes (character arrays).
 */
class SerialConnection {
protected:
    bool connected;

public:
    explicit SerialConnection(const std::string port);
    ~SerialConnection();

    int read_data(const char* buffer, unsigned int buffer_size);
    bool write_data(const char* buffer, unsigned int buffer_size);

    void close();
};