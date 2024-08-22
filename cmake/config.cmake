
set(ENABLE_PYTHON TRUE)


set(LOCAL_CONFIG_FILE ${CMAKE_SOURCE_DIR}/cmake/config_local.cmake)
if(EXISTS "${LOCAL_CONFIG_FILE}")
    message(STATUS "overwriting default configurations by loading local file ...")
    include(${LOCAL_CONFIG_FILE})
endif()
unset(LOCAL_CONFIG_FILE)
