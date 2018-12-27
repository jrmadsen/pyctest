
set(COPY_DIR ${PROJECT_BINARY_DIR}/examples/TomoPy)
set(FILES README.md pyctest_tomopy.py)

foreach(_FILE ${FILES})
    configure_file(${_FILE} ${COPY_DIR}/${_FILE} COPYONLY)
endforeach(_FILE ${FILES})
