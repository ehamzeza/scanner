#include "obj_writer.hpp"

void write_obj_file(const ScanData &scan_data, const std::string output_filepath)
{
    const int num_rows = (int)(scan_data.scan_height / scan_data.scan_z_resolution);
    const int points_per_row = (int)(360.0f / scan_data.scan_arc_resolution);

    std::cout << "Writing obj file..." << std::endl;
    std::cout << "\t num_rows = " << num_rows << std::endl;
    std::cout << "\t point_per_row = " << points_per_row << std::endl;

    std::ofstream file(output_filepath);

    if (file.fail())
    {
        std::cerr << "Failed to open file for writing..." << std::endl;
        return;
    }

    // Write the vertex information
    for (const glm::vec3 &vertex : scan_data.scanned_points)
        file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;

    // Write the face information
    for (int i = 0; i < points_per_row - 1; i++)
    {
        for (int j = 0; j < num_rows - 1; j++)
        {
            int points[4] = {
                get_index(i, j, points_per_row),
                get_index(i + 1, j, points_per_row),
                get_index(i + 1, j + 1, points_per_row),
                get_index(i, j + 1, points_per_row)
            };

            file << "f " << points[0] << " " << points[1] << " " << points[2] << std::endl;
            file << "f " << points[0] << " " << points[2] << " " << points[3] << std::endl;
        }
    }

    // Fill in the seam of the unwrapped cyllinder
    for (int j = 0; j < num_rows - 1; j++)
    {
        int points[4] = {
            get_index(points_per_row - 1, j, points_per_row),
            get_index(points_per_row - 1, j + 1, points_per_row),
            get_index(0, j, points_per_row),
            get_index(0, j + 1, points_per_row)
        };
        file << "f " << points[0] << " " << points[2] << " " << points[3] << std::endl;
        file << "f " << points[0] << " " << points[3] << " " << points[1] << std::endl;
    }

    // Fill in the top of the cyllinder
    file << "f ";
    for (int i = 0; i < points_per_row; i++)
    {
        file << get_index(i, num_rows - 1, points_per_row);
        file << " ";
    }
    file << std::endl;

    // Fill in the bottom of the cyllinder
    file << "f ";
    for (int i = 0; i < points_per_row; i++)
    {
        file << get_index(i, 0, points_per_row);
        file << " ";
    }
    file << std::endl;

    file.flush();
    file.close();
}