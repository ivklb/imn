
add_library(core
    app.cpp
    io.cpp
    setting.cpp
)

target_include_directories(core PUBLIC ${SRC_ROOT})

target_link_libraries(core PRIVATE
    nlohmann_json::nlohmann_json
)
