#pragma once

#include <glm/vec3.hpp>

#include <string>
#include <vector>
#include <queue>

struct
{
    std::string command;
    bool sent = false;

    float row_height = 0.0f;
    float arc_offset = 0.0f;
} typedef ScanCommand;

struct
{
    bool scan_started = false;
    float scan_height = 0.0f;          // milimeters
    float scan_z_resolution = 10.0f;   // milimeters
    float scan_arc_resolution = 36.0f; // degrees
    std::vector<glm::vec3> scanned_points;
    std::queue<ScanCommand> command_queue;
} typedef ScanData;