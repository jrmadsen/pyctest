
file(GLOB tomopy_files "${CMAKE_CURRENT_LIST_DIR}/TomoPy/*")

foreach(_FILE ${tomopy_files})
    get_filename_component(_FILE_BASE "${_FILE}" NAME)
    configure_file(${_FILE} ${PROJECT_BINARY_DIR}/examples/TomoPy/${_FILE_BASE}
        COPYONLY)
endforeach(_FILE ${tomopy_files})
