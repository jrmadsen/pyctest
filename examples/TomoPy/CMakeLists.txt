
set(COPY_DIR ${PROJECT_BINARY_DIR}/examples/TomoPy)
set(FILES
    PyCTestPreInit.cmake
    README.md
    run_tomopy.py
    tomopy.py
    tomopy_rec.py
    tomopy_test_utils.py
)

foreach(_FILE ${FILES})
    configure_file(${_FILE} ${COPY_DIR}/${_FILE} COPYONLY)
endforeach(_FILE ${FILES})
