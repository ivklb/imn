
add_library(
    node_editor
    imnodes.cpp
)

target_include_directories(node_editor PUBLIC ${SRC_ROOT})

target_link_libraries(node_editor
    imgui::imgui
)
