################################################################################
#
#        Configuration
#
################################################################################

get_property(CMAKE_PROJECT_LIBRARIES GLOBAL PROPERTY CMAKE_PROJECT_LIBRARIES)
get_property(CTEST_PROJECT_LIBRARIES GLOBAL PROPERTY CTEST_PROJECT_LIBRARIES)

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/source/kitware-cmake/Source)
include_directories(SYSTEM ${PROJECT_BINARY_DIR}/source/kitware-cmake/Source)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/source/kitware-cmake/Utilities)
include_directories(SYSTEM ${PROJECT_BINARY_DIR}/source/kitware-cmake/Utilities)

execute_process(COMMAND ${PYTHON_EXECUTABLE}
    -c "import sys; print('{}'.format(sys.version_info[0]))"
    OUTPUT_VARIABLE PYTHON_MAJOR_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
add_definitions(-D_PYTHON_MAJOR_VERSION=${PYTHON_MAJOR_VERSION})

file(GLOB_RECURSE pybind_headers ${CMAKE_CURRENT_LIST_DIR}/pybind11/include/*.h)

################################################################################
#
#        PyBind11
#
################################################################################
pybind11_add_module(pycmake
    ${CMAKE_CURRENT_LIST_DIR}/pycmake.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pycmake.hpp
    ${CMAKE_CURRENT_LIST_DIR}/cmcmd.hpp
    ${CMAKE_CURRENT_LIST_DIR}/cmcmd.cpp
    ${pybind_headers})

target_link_libraries(pycmake PUBLIC
    ${CMAKE_PROJECT_LIBRARIES})

set_target_properties(pycmake
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pyctest
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pyctest
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pyctest)

install(TARGETS pycmake DESTINATION ${CMAKE_INSTALL_PYTHONDIR}
    COMPONENT python)

pybind11_add_module(pyctest
    ${CMAKE_CURRENT_LIST_DIR}/pyctest.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pyctest.hpp
    ${CMAKE_CURRENT_LIST_DIR}/pycmExecuteProcessCommand.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pycmExecuteProcessCommand.hpp
    ${pybind_headers})

target_link_libraries(pyctest PUBLIC
    ${CTEST_PROJECT_LIBRARIES} ${CMAKE_PROJECT_LIBRARIES})

set_target_properties(pyctest
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pyctest
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pyctest
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pyctest)

install(TARGETS pyctest DESTINATION ${CMAKE_INSTALL_PYTHONDIR}
    COMPONENT python)

################################################################################
#
#        Python package files
#
################################################################################
# glob all files
file(GLOB_RECURSE PACKAGE_FILES ${PROJECT_SOURCE_DIR}/pyctest/*.py.in)

# process package files
foreach(_IN_FILE ${PACKAGE_FILES})

    # get the relative path
    file(RELATIVE_PATH _FILE_RELATIVE ${PROJECT_SOURCE_DIR}/pyctest ${_IN_FILE})

    # get basename of file
    get_filename_component(_IN_NAME ${_IN_FILE} NAME_WE)
    # find if there is a directory underneath
    get_filename_component(_REL_DIR ${_FILE_RELATIVE} PATH)

    # if subdirectory
    if(NOT "${_REL_DIR}" STREQUAL "")
        set(_OUT_PATH ${CMAKE_BINARY_DIR}/pyctest/${_REL_DIR})
    else()
    # if no subdirectory
        set(_OUT_PATH ${CMAKE_BINARY_DIR}/pyctest)
    endif()

    set(_OUT_FILE ${_OUT_PATH}/${_IN_NAME}.py)
    message(STATUS "CONFIGURING OUTPUT FILE: ${_OUT_FILE}")
    configure_file(${_IN_FILE} ${_OUT_FILE} @ONLY)
    install(FILES ${_OUT_FILE} DESTINATION ${CMAKE_INSTALL_PYTHONDIR}
        COMPONENT python)

endforeach()
