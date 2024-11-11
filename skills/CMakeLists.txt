
project(skills)


set(_Example_Sources
        skills.cpp
        skills.h
        nodes.cpp
        nodes.h
        register.cpp
        utilities/builders.h
        utilities/drawing.h
        utilities/widgets.h
        utilities/builders.cpp
        utilities/drawing.cpp
        utilities/widgets.cpp
)

#source_group("" FILES ${_Example_Sources})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_Example_Sources})

file(GLOB _Example_CommonResources CONFIGURE_DEPENDS "${IMGUI_NODE_EDITOR_ROOT_DIR}/skills/data/*")
file(GLOB _Example_Resources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/*")
#message(FATAL_ERROR "_Example_Resources = ${_Example_Resources}")

add_executable(skills ${_Example_Sources} ${_Example_Resources} ${_Example_CommonResources})

find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)
target_link_libraries(skills PRIVATE imgui imgui_node_editor application)

set(_ExampleBinDir ${CMAKE_BINARY_DIR}/bin)

set_target_properties(skills PROPERTIES
        FOLDER "skills-app"
        RUNTIME_OUTPUT_DIRECTORY                "${_ExampleBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${_ExampleBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_ExampleBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${_ExampleBinDir}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${_ExampleBinDir}"
        DEBUG_POSTFIX                           _d
        RELWITHDEBINGO_POSTFIX                  _rd
        MINSIZEREL_POSTFIX                      _r
        VS_DEBUGGER_WORKING_DIRECTORY           ${_ExampleBinDir}
        MACOSX_BUNDLE_INFO_PLIST                "${IMGUI_NODE_EDITOR_ROOT_DIR}/skills/application/support/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME               "${PACKAGE_NAME}"
        MACOSX_BUNDLE_GUI_IDENTIFIER            "com.sandbox.collisions"
        MACOSX_BUNDLE_LONG_VERSION_STRING       "${PACKAGE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING      "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}"
        MACOSX_BUNDLE_ICON_FILE                 Icon.icns
)

add_custom_command(
        TARGET skills
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ARGS ${_ExampleBinDir}/data
)

set(_ResourceRoot ${CMAKE_CURRENT_SOURCE_DIR})
foreach(_Resource ROOT "${IMGUI_NODE_EDITOR_ROOT_DIR}/examples/data" ${_Example_CommonResources} ROOT "${CMAKE_CURRENT_SOURCE_DIR}/data" ${_Example_Resources})
    if (_Resource STREQUAL ROOT)
        set(_ResourceRoot FALSE)
        continue()
    elseif(NOT _ResourceRoot)
        set(_ResourceRoot ${_Resource})
        continue()
    endif()

    if ("${_Resource}" MATCHES "\.DS_Store$")
        list(REMOVE_ITEM _Example_Resources ${_Resource})
        list(REMOVE_ITEM _Example_CommonResources ${_Resource})
        continue()
    endif()

    file(RELATIVE_PATH _RelResource ${_ResourceRoot} ${_Resource})

    add_custom_command(
            TARGET skills
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ARGS ${_Resource} ${_ExampleBinDir}/data/${_RelResource}
    )
endforeach()

add_subdirectory(application)
