project(imgui_node_editor)

find_package(imgui REQUIRED)

set(_imgui_node_editor_Sources
        crude_json.cpp
        crude_json.h
        imgui_bezier_math.h
        imgui_bezier_math.inl
        imgui_canvas.cpp
        imgui_canvas.cpp
        imgui_canvas.h
        imgui_canvas.h
        imgui_extra_math.h
        imgui_extra_math.inl
        imgui_node_editor_api.cpp
        imgui_node_editor_internal.h
        imgui_node_editor_internal.inl
        imgui_node_editor.cpp
        imgui_node_editor.h
        ${IMGUI_NODE_EDITOR_ROOT_DIR}/misc/imgui_node_editor.natvis
)

add_library(imgui_node_editor STATIC
        ${_imgui_node_editor_Sources}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(imgui_node_editor PUBLIC
        ${IMGUI_NODE_EDITOR_ROOT_DIR}/external/imgui_node_editor/
)

target_link_libraries(imgui_node_editor PUBLIC imgui)

source_group(TREE ${IMGUI_NODE_EDITOR_ROOT_DIR} FILES ${_imgui_node_editor_Sources})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "external")