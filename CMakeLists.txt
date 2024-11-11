cmake_minimum_required(VERSION 3.12)

project(imgui-node-editor)

# Define IMGUI_NODE_EDITOR_ROOT_DIR pointing to project root directory
get_filename_component(IMGUI_NODE_EDITOR_ROOT_DIR ${CMAKE_SOURCE_DIR}/ ABSOLUTE CACHE)

# Enable solution folders in Visual Studio and Folders in Xcode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Point CMake where to look for module files.
list(APPEND CMAKE_MODULE_PATH ${IMGUI_NODE_EDITOR_ROOT_DIR}/misc/cmake-modules)

# Node editor use C++14
set(CMAKE_CXX_STANDARD            14)
set(CMAKE_CXX_STANDARD_REQUIRED   YES)

add_subdirectory(skills)