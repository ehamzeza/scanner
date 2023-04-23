cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_BUILD_TYPE Debug)


# Define the project and add the source files
set(PROJECT_NAME "scanner")
project(${PROJECT_NAME})

add_executable(${PROJECT_NAME}
    "src/main.cpp"
    "src/application/core/viewport.cpp"
    "src/application/core/application.cpp"
    "src/application/core/usb_interface.cpp"
    "src/application/logger/logger.cpp"
    "src/application/serial/serial.cpp"
    # "src/application/serial/serial-win32.cpp"
    "src/application/serial/serial-linux.cpp"
    
    # Shaders
    "src/application/shaders/basic_shader.cpp"
    "src/application/shaders/line_shader.cpp"

    # Graphics Engine
    "src/graphics/core/camera/camera.cpp"
    "src/graphics/core/opengl/buffers.cpp"
    "src/graphics/core/opengl/shaders.cpp"
    "src/graphics/core/model/model.cpp"
    "src/graphics/core/mesh/mesh.cpp"
    "src/graphics/display/window/window.cpp"
    "src/graphics/display/surface/surface.cpp"
    "src/graphics/utility/loader.cpp"
    "src/graphics/utility/utility.cpp"
    "src/graphics/utility/mouse.cpp"
    "src/graphics/utility/math.cpp")

# if (WIN32)
#     add_executable(${PROJECT_NAME} "src/application/serial/serial-win32.cpp")
# endif(WIN32)

# if (UNIX)
#     add_executable(${PROJECT_NAME} "src/application/serial/serial-linux.cpp")
# endif(UNIX)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    "src"
    "src/application"
    "src/application/core"
    "src/application/logger"
    "src/application/serial"

    # Shaders
    "src/application/shaders"

    # Graphics Engine
    "src/graphics"
    "src/graphics/core"
    "src/graphics/core/mesh"
    "src/graphics/core/model"
    "src/graphics/core/camera"
    "src/graphics/core/opengl"
    "src/graphics/display/window"
    "src/graphics/display/surface"
    "src/graphics/utility")

# sudo apt install libudev-dev
# find_library(UDEV_LIBRARY NAMES udev REQUIRED)
# target_link_libraries(${PROJECT_NAME} PUBLIC ${UDEV_LIBRARY})

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

# Add GLM subdirectory and link slicer against it.
add_subdirectory(lib/glm lib-build/glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)

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