#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "scan_data.hpp"

inline int get_index(int i, int j, int points_per_row) { return i + (j * points_per_row) + 1; }

void write_obj_file(const ScanData &scan_data, const std::string output_filepath);
