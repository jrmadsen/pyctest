################################################################################
#
#        Creates a 'format' target that runs clang-format
#
################################################################################

find_program(CLANG_FORMATTER
    NAMES
        clang-format-8.0
        clang-format-7.0
        clang-format-6.0
        clang-format)

if(CLANG_FORMATTER)
    set(_CMake_DIR ${PROJECT_SOURCE_DIR}/source/kitware-cmake/Source)
    set(_Python_DIR ${PROJECT_SOURCE_DIR}/source/python)

    file(GLOB py_headers ${_Python_DIR}/*.hpp)
    file(GLOB py_sources ${_Python_DIR}/*.cpp)

    file(GLOB cm_headers ${_CMake_DIR}/*.h ${_CMake_DIR}/*.hxx
        ${_CMake_DIR}/CTest/*.h ${_CMake_DIR}/CPack/*.h)
    file(GLOB cm_sources ${_CMake_DIR}/*.c ${_CMake_DIR}/*.cxx
        ${_CMake_DIR}/CTest/*.cxx ${_CMake_DIR}/CPack/*.cxx)

    add_custom_target(format
        COMMAND ${CLANG_FORMATTER} -i ${py_headers} ${py_sources}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Running '${CLANG_FORMATTER}' on '${_Python_DIR}'..."
        SOURCES ${py_headers} ${py_sources})

    add_custom_target(format-cmake
        COMMAND ${CLANG_FORMATTER} -i ${cm_headers} ${cm_sources}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Running '${CLANG_FORMATTER}' on '${_CMake_DIR}'..."
        SOURCES ${cm_headers} ${cm_sources})

endif()
