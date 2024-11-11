if (TARGET imgui_node_editor)
    return()
endif()

set(_imgui_node_editor_SourceDir ${IMGUI_NODE_EDITOR_ROOT_DIR}/external/imgui_node_editor/)
set(_imgui_node_editor_BinaryDir ${CMAKE_BINARY_DIR}/imgui_node_editor)

add_subdirectory(${_imgui_node_editor_SourceDir} ${_imgui_node_editor_BinaryDir})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    imgui_node_editor
    REQUIRED_VARS
        IMGUI_NODE_EDITOR_ROOT_DIR
)