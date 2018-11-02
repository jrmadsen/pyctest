#------------------------------------------------------------------------------#
#
#   The PyCTestPreInit.cmake script is called before any build, tests, etc.
#   are run.
#
#   This script will be copied over to pyctest.pyctest.BINARY_DIRECTORY
#   so relative paths should be w.r.t. to the BINARY_DIRECTORY
#
#   The intention of this script is to setup the build + testing environment
#
#   - J. Madsen (July 5th, 2018)
#------------------------------------------------------------------------------#


#------------------------------------------------------------------------------#
#   echo message for debug
#
message(STATUS "Including ${CMAKE_CURRENT_LIST_FILE}...")


#------------------------------------------------------------------------------#
# provides macros:
#   download_conda(...)
#   find_conda(...)
#   configure_conda(...)
#
include("${CMAKE_CURRENT_LIST_DIR}/Utilities.cmake")


#------------------------------------------------------------------------------#
# Determine Python version
#------------------------------------------------------------------------------#
macro(SET_PYTHON_VERSION VAL)
    if(NOT DEFINED PYTHON_VERSION OR "${PYTHON_VERSION}" STREQUAL "")
        if(NOT "${VAL}" STREQUAL "")
            set(PYTHON_VERSION "${VAL}")
        endif(NOT "${VAL}" STREQUAL "")
    endif(NOT DEFINED PYTHON_VERSION OR "${PYTHON_VERSION}" STREQUAL "")
endmacro(SET_PYTHON_VERSION VAL)

set_python_version("$ENV{PYTHON_VERSION}")
set_python_version("$ENV{TRAVIS_PYTHON_VERSION}")
set_python_version("2.7")


#------------------------------------------------------------------------------#
# set some values
#------------------------------------------------------------------------------#
set(ENV{PYTHONPATH} "") # conda doesn't like PYTHONPATH
# control whether to download + install when not necessary
set(CONDA_PACKAGE_INSTALL ON)
# prefix for miniconda install
set(CONDA_PREFIX "${CMAKE_CURRENT_LIST_DIR}/Miniconda")
# environment to install to
set(CONDA_ENVIRONMENT "tomopy-pyctest")
# packages
set(CONDA_PACKAGES
    nose six numpy h5py scipy scikit-image pywavelets mkl-devel
    mkl_fft python-coveralls dxchange numexpr timemory)

if(NOT "$ENV{CONDA_PREFIX}" STREQUAL "")
    set(CONDA_PREFIX $ENV{CONDA_PREFIX})
endif()

if(NOT "$ENV{CONDA_ENVIRONMENT}" STREQUAL "")
    set(CONDA_ENVIRONMENT $ENV{CONDA_ENVIRONMENT})
endif()

if(NOT "$ENV{CONDA_EXE}" STREQUAL "")
    set(CONDA_EXE ${CONDA_EXE})
else()
    set(CONDA_EXE ${CONDA_PREFIX}/bin/conda)
endif()

if(NOT "$ENV{CONDA_PACKAGE_INSTALL}" STREQUAL "")
    set(CONDA_PACKAGE_INSTALL $ENV{CONDA_PACKAGE_INSTALL})
endif()

if("${PYTHON_VERSION}" VERSION_EQUAL "2.7")
    list(APPEND CONDA_PACKAGES "futures")
endif()

message(STATUS "")
message(STATUS "[${CMAKE_CURRENT_LIST_FILE}]")
message(STATUS "")
message(STATUS "    Python version    : ${PYTHON_VERSION}")
message(STATUS "    Conda prefix      : ${CONDA_PREFIX}")
message(STATUS "    Conda environment : ${CONDA_ENVIRONMENT}")
message(STATUS "    Conda executable  : ${CONDA_EXE}")
message(STATUS "")

#------------------------------------------------------------------------------#
#   download Miniconda if not already exists
#------------------------------------------------------------------------------#
# if not already installed
if(NOT EXISTS "${CONDA_EXE}")
    message(STATUS "CONDA_EXE (${CONDA_EXE}) does not exist. Downloading conda...")

    download_conda(
        VERSION "latest"
        PYTHON_VERSION ${PYTHON_VERSION}
        INSTALL_PREFIX ${CONDA_PREFIX}
        ARCH "x86_64"
        DOWNLOAD_DIR "${CMAKE_CURRENT_LIST_DIR}")

    set(CONDA_PACKAGE_INSTALL ON)

endif(NOT EXISTS "${CONDA_EXE}")


#------------------------------------------------------------------------------#
#   setup -- use CONDA_PACKAGE_INSTALL to avoid reinstalling if not necessary
#------------------------------------------------------------------------------#
if(CONDA_PACKAGE_INSTALL)

    message(STATUS "")
    message(STATUS "Configuring conda:")
    message(STATUS "    prefix          : ${CONDA_PREFIX}")
    message(STATUS "    python version  : ${PYTHON_VERSION}")
    message(STATUS "    environment     : ${CONDA_ENVIRONMENT}")
    message(STATUS "    packages        : ${CONDA_PACKAGES}")
    message(STATUS "")

    configure_conda(
        PYTHON_VERSION      ${PYTHON_VERSION}
        PREFIX              ${CONDA_PREFIX}
        ENVIRONMENT         ${CONDA_ENVIRONMENT}
        PACKAGES            ${CONDA_PACKAGES}
        CHANNELS            conda-forge jrmadsen)

endif()


#------------------------------------------------------------------------------#
#   ensure environment setup
#------------------------------------------------------------------------------#
find_conda(${CONDA_PREFIX} ${CONDA_ENVIRONMENT})
