
add_library(core
    app.cpp
    backend.cpp
    io.cpp
    log.cpp
    python.cpp
    setting.cpp
)

target_include_directories(core PUBLIC ${SRC_ROOT})

target_link_libraries(core PRIVATE
    nlohmann_json::nlohmann_json
    pybind11::embed
)
