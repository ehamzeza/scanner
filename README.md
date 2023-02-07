# About
Scanner is a project designed to interface over USB with custom hardware to retrieve point cloud data and provide visualization tools. Currently, the project is only supported on linux (tested on Ubuntu 20.04).

# Cloning Repository

Simply clone with the recursive flag in order to fetch the required submodules.
```
git clone --recursive https://github.com/ehamzeza/scanner.git
```

# Dependencies

## GLFW: (Submodule)
A lightweight utility library being used to create and manage windows in a cross-platform build environment. <br>
<b>Github Repository: </b>https://github.com/glfw/glfw.git<br>

If you experience errors with building GLFW on Ubuntu you most likely lack certain system headers that can be installed with the following sequence of commands.
```
sudo apt update
sudo apt install libxinerama-dev
sudo apt install libxcursor-dev
sudo apt install libxrandr-dev
sudo apt install libxi-dev
```

## GLEW: (Submodule)
A cross-platform OpenGL extension loading system that enables Penta Print to use modern OpenGL on all supported systems. <br>
<b>Github Repository: </b>https://github.com/Perlmint/glew-cmake.git<br>

## GLM: (Submodule)
A linear algebra mathematics library designed to work closely with OpenGL. <br>
<b>Github Repository: </b>https://github.com/g-truc/glm.git<br>

## Dear ImGui: (Submodule)
A graphical user interface library used to manage all of the UI in a hardware-accelerated OpenGL environment. Note that this project is currently using the `docking` branch of the linked repository.<br>
<b>Github Repository: </b> https://github.com/ocornut/imgui.git<br>


## LibUSB: (System Install)
Many linux distributions come with LibUSB already installed. If not, you can run the following commands.
**Github Repository:** https://github.com/libusb/libusb
```
sudo apt update
sudo apt install libusb-1.0-0-dev
```
*Note: If you recieve linker errors when building using `make` with the `libusb-1.0-0-dev` package, you may try using `libusb-dev` instead.*