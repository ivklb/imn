# TODO: make compatible with linux

if(NOT EXISTS "${TARGET_DIR}/python.exe")
    message(STATUS "install python... ${PY_DIR} -> ${TARGET_DIR}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${PY_DIR} ${TARGET_DIR}
    )
endif()

if(NOT EXISTS "${TARGET_DIR}/Scripts/pip.exe")
    message(STATUS "install pip...")
    include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)
    set(PIP_URL "https://bootstrap.pypa.io/get-pip.py")
    set(TIMEOUT_SECONDS 600)
    download_if_not_exists(${PIP_URL} ${TARGET_DIR} ${TIMEOUT_SECONDS})
    execute_process(COMMAND ${TARGET_DIR}/python "${TARGET_DIR}/get-pip.py")
endif()

message(STATUS "running pip install...")
execute_process(COMMAND
    "${TARGET_DIR}/python" "-m" "pip" "install" "-r" "${CMAKE_SOURCE_DIR}/requirements.txt" "--no-warn-script-location"
)
