
function(download_if_not_exists URL TARGET_DIR DOWNLOAD_TIMEOUT)
    get_filename_component(FILENAME "${URL}" NAME)
    find_file(FOUND_FILENAME
        NAMES ${FILENAME}
        PATHS ${TARGET_DIR}
        NO_DEFAULT_PATH
    )

    if(NOT FOUND_FILENAME)
        message(STATUS "Downloading ${URL} to ${TARGET_DIR}")

        file(DOWNLOAD ${URL}
            ${TARGET_DIR}/${FILENAME}
            TIMEOUT ${DOWNLOAD_TIMEOUT}
            STATUS ERR
            # SHOW_PROGRESS
        )

        if(ERR EQUAL 0)
        else()
            message(FATAL_ERROR "Download failed, error: ${ERR}")
        endif()
    endif()
endfunction()
