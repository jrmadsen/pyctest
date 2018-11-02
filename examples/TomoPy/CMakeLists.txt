
set(COPY_DIR ${PROJECT_BINARY_DIR}/examples/TomoPy)
set(FILES
    # info
    README.md
    # configures conda
    PyCTestPreInit.cmake
    # drivers
    pyctest_tomopy.py
    pyctest_tomopy.sh
    # reconstruction files
    pyctest_tomopy_phantom.py
    pyctest_tomopy_rec.py
    # helpers
    pyctest_tomopy_utils.py
)

foreach(_FILE ${FILES})
    configure_file(${_FILE} ${COPY_DIR}/${_FILE} COPYONLY)
endforeach(_FILE ${FILES})
