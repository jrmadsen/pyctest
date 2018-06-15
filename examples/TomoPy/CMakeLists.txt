
set(COPY_DIR ${PROJECT_BINARY_DIR}/examples/TomoPy)
set(FILES
    PyCTestPreInit.cmake
    README.md
    measurement.py
    run_tomopy.py
    tomopy.py
)

foreach(_FILE ${FILES})
    configure_file(${_FILE} ${COPY_DIR}/${_FILE} COPYONLY)
endforeach(_FILE ${FILES})
