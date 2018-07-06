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
set(MINICONDA_PACKAGE_INSTALL
    OFF)
# prefix for miniconda install
set(MINICONDA_PREFIX
    "${CMAKE_CURRENT_LIST_DIR}/miniconda")
# environment to install to
set(MINICONDA_ENVIRONMENT "root")
# packages
set(MINICONDA_PACKAGES
    nose six numpy h5py scipy scikit-image pywavelets mkl-devel
    mkl_fft python-coveralls dxchange numexpr timemory)

if(NOT "$ENV{CONDA_PACKAGE_INSTALL}" STREQUAL "" OR
   NOT "$ENV{MINICONDA_PACKAGE_INSTALL}" STREQUAL "")
    set(MINICONDA_PACKAGE_INSTALL ON)
endif()

if("${PYTHON_VERSION}" VERSION_EQUAL "2.7")
    list(APPEND MINICONDA_PACKAGES "futures")
endif()


#------------------------------------------------------------------------------#
#   download Miniconda if not already exists
#------------------------------------------------------------------------------#
# if not already installed
if(NOT EXISTS "${MINICONDA_PREFIX}/bin/conda")

    download_conda(
        VERSION "latest"
        PYTHON_VERSION ${PYTHON_VERSION}
        INSTALL_PREFIX ${MINICONDA_PREFIX}
        ARCH "x86_64"
        DOWNLOAD_DIR "${CMAKE_CURRENT_LIST_DIR}")

    set(MINICONDA_PACKAGE_INSTALL ON)

endif(NOT EXISTS "${MINICONDA_PREFIX}/bin/conda")


#------------------------------------------------------------------------------#
#   setup -- use MINICONDA_PACKAGE_INSTALL to avoid reinstalling if not necessary
#------------------------------------------------------------------------------#
if(MINICONDA_PACKAGE_INSTALL)
    configure_conda(UPDATE
        PYTHON_VERSION      ${PYTHON_VERSION}
        PREFIX              ${MINICONDA_PREFIX}
        ENVIRONMENT         ${MINICONDA_ENVIRONMENT}
        PACKAGES            ${MINICONDA_PACKAGES}
        CHANNELS            conda-forge jrmadsen)

    find_conda(${MINICONDA_PREFIX} ${MINICONDA_ENVIRONMENT})
    # tomopy has errors if install is not called a second time on KNL
    execute_process(COMMAND ${CONDA_EXE} install -n ${MINICONDA_ENVIRONMENT}
        python=${PYTHON_VERSION} ${MINICONDA_PACKAGES})
endif()

find_conda(${MINICONDA_PREFIX} ${MINICONDA_ENVIRONMENT})
