cmake_minimum_required(VERSION 3.0.0)

# set(PROJECT_NAME scanner)

# Define the project and add the files
project(scanner)
add_executable(scanner src/main.cpp)

find_library(usb-1.0 REQUIRED)
target_link_libraries(scanner PUBLIC usb-1.0)

