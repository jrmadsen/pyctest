cmake_minimum_required(VERSION 2.8.12)

set(COPY_DIR ${PROJECT_BINARY_DIR}/examples/Basic)
set(FILES
    README.md
    basic.py
)

foreach(_FILE ${FILES})
    configure_file(${_FILE} ${COPY_DIR}/${_FILE} COPYONLY)
endforeach(_FILE ${FILES})
