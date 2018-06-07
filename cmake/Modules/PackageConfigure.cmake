
################################################################################
#
#        PYCTEST Package installation
#
################################################################################

include(CMakePackageConfigHelpers)

set(PYTHON_INSTALL_DIR      ${PYCTEST_CONFIG_PYTHONDIR})
set(INCLUDE_INSTALL_DIR     ${CMAKE_INSTALL_INCLUDEDIR})
set(LIB_INSTALL_DIR         ${CMAKE_INSTALL_LIBDIR})

set(_INSTALL_PREFIX ${PYCTEST_INSTALL_PREFIX})
execute_process(COMMAND
    ${PYTHON_EXECUTABLE} -c "import sys; print('{}'.format(sys.prefix))"
    OUTPUT_VARIABLE _INSTALL_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/Templates/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${PYCTEST_INSTALL_CMAKEDIR}
    INSTALL_PREFIX ${_INSTALL_PREFIX}
    PATH_VARS
        INCLUDE_INSTALL_DIR
        LIB_INSTALL_DIR
        PYTHON_INSTALL_DIR)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
