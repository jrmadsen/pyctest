
################################################################################
#
#                               CMake
#
################################################################################
# checkout CMake if not checked out
checkout_cmake_submodule(RECURSIVE
    RELATIVE_PATH kitware-cmake
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/source)

################################################################################
#
#                               PyBind11
#
################################################################################

# checkout PyBind11 if not checked out
checkout_cmake_submodule(RECURSIVE
    RELATIVE_PATH pybind11
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/source/python)

# make sure pybind11 gets installed in same place as PYCTEST
if(PYBIND11_INSTALL)
    set(PYBIND11_CMAKECONFIG_INSTALL_DIR
        "${PYCTEST_INSTALL_DATAROOTDIR}/cmake/pybind11"
        CACHE STRING "install path for pybind11Config.cmake" FORCE)
    set(CMAKE_INSTALL_INCLUDEDIR ${PYCTEST_INSTALL_INCLUDEDIR}
        CACHE PATH "Include file installation path" FORCE)
endif(PYBIND11_INSTALL)

# C++ standard
set(PYBIND11_CPP_STANDARD -std=c++${CMAKE_CXX_STANDARD}
    CACHE STRING "PyBind11 CXX standard" FORCE)

# add PyBind11 to project
add_subdirectory(${PROJECT_SOURCE_DIR}/source/python/pybind11)

if(NOT PYBIND11_PYTHON_VERSION)
    execute_process(COMMAND ${PYTHON_EXECUTABLE}
        -c "import sys; print('{}.{}'.format(sys.version_info[0], sys.version_info[1]))"
        OUTPUT_VARIABLE PYTHON_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "Python version: ${PYTHON_VERSION}")
    set(PYBIND11_PYTHON_VERSION "${PYTHON_VERSION}"
        CACHE STRING "Python version" FORCE)
endif(NOT PYBIND11_PYTHON_VERSION)

add_feature(PYBIND11_PYTHON_VERSION "PyBind11 Python version")


################################################################################
#
#   Python installation directories
#
################################################################################

if(PYCTEST_SETUP_PY)

    set(CMAKE_INSTALL_PYTHONDIR ${CMAKE_INSTALL_PREFIX} CACHE PATH
        "Installation prefix of python" FORCE)

else(PYCTEST_SETUP_PY)

    set(CMAKE_INSTALL_PYTHONDIR
        ${CMAKE_INSTALL_LIBDIR}/python${PYBIND11_PYTHON_VERSION}/site-packages/pyctest
        CACHE PATH "Installation directory for python")

endif(PYCTEST_SETUP_PY)

add_feature(CMAKE_INSTALL_PYTHONDIR "pyctest Python installation directory")

execute_process(COMMAND ${PYTHON_EXECUTABLE}
        -c "import time ; print('{} {}'.format(time.ctime(), time.tzname[0]))"
        OUTPUT_VARIABLE PYCTEST_INSTALL_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)

################################################################################
#
#        External variables
#
################################################################################

# including the directories
safe_remove_duplicates(EXTERNAL_INCLUDE_DIRS ${EXTERNAL_INCLUDE_DIRS})
safe_remove_duplicates(EXTERNAL_LIBRARIES ${EXTERNAL_LIBRARIES})
foreach(_DIR ${EXTERNAL_INCLUDE_DIRS})
    include_directories(SYSTEM ${_DIR})
endforeach(_DIR ${EXTERNAL_INCLUDE_DIRS})
