cmake_minimum_required(VERSION 3.1.0)
set(PROJECT_NAME "scanner")


# Define the project and add the source files
project(${PROJECT_NAME})
add_executable(${PROJECT_NAME}
    "src/main.cpp"
    "src/window.cpp")
target_include_directories(${PROJECT_NAME}
    PRIVATE
    "src")

# Find and link against non-cmake library libusb
find_library(usb-1.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC usb-1.0)


# We do not want to build the GLFW tests, examples, and 
# documentation when we are building the slicer.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add the submodule to the build, this creates the target
# "glfw" which we can link against.
add_subdirectory(lib/glfw lib-build/glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)


# Add glew to the build and link against it
add_subdirectory(lib/glew lib-build/glew)
target_link_libraries(${PROJECT_NAME} PUBLIC libglew_static)


# ImGui library is provided as source files, build it 
# differently and link against it.
SET(IMGUI_TARGET "ImGui")
file(GLOB IMGUI_SOURCES 
    lib/imgui/*.cpp
    # The project is only using the OpenGL 3.0 and GLFW backends, so
    # there is no need to compile the remaining backend sources.
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp)
add_library(${IMGUI_TARGET} STATIC ${IMGUI_SOURCES})
target_include_directories(${IMGUI_TARGET} PUBLIC lib/imgui)
target_link_libraries(${IMGUI_TARGET} PUBLIC glfw)

# Adding imgui to scanner
target_link_libraries(${PROJECT_NAME} PUBLIC ${IMGUI_TARGET})