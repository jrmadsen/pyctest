
message(STATUS "Including ${CMAKE_CURRENT_LIST_FILE}...")

find_program(DEFAULT_CONDA_EXE NAMES conda)

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

find_program(WGET_EXE NAMES wget)
find_program(BASH_EXE NAMES bash)

set(CONDA_PACKAGE_INSTALL OFF)
if(NOT "$ENV{CONDA_PACKAGE_INSTALL}" STREQUAL "")
    set(CONDA_PACKAGE_INSTALL ON)
endif()
set(ENV{PYTHONPATH} "")
set(CONDA_PKGS nose six numpy h5py scipy scikit-image pywavelets mkl-devel
    mkl_fft python-coveralls dxchange numexpr timemory scikit-image)
#set(PIP_PKGS timemory)

#------------------------------------------------------------------------------#
#   local `miniconda` installation
#------------------------------------------------------------------------------#
# if not already installed
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/conda")

    set(CONDA_PACKAGE_INSTALL ON)
    if("${PYTHON_VERSION}" VERSION_EQUAL "2.7")
        set(MINICONDA_VER "Miniconda")
        list(APPEND CONDA_PKGS "futures")
    else()
        set(MINICONDA_VER "Miniconda3")
    endif()

    if(APPLE)
        set(_OS "MacOSX")
    elseif(WIN32)
        set(_OS "Windows")
    else()
        set(_OS "Linux")
    endif(APPLE)

    set(MINICONDA_URL "https://repo.continuum.io/miniconda/${MINICONDA_VER}-latest-${_OS}-x86_64.sh")
    execute_process(COMMAND
        ${WGET_EXE} ${MINICONDA_URL} -O ${CMAKE_CURRENT_LIST_DIR}/miniconda.sh)
    execute_process(COMMAND
        ${BASH_EXE} ${CMAKE_CURRENT_LIST_DIR}/miniconda.sh -b -p ${CMAKE_CURRENT_LIST_DIR}/miniconda)

endif(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/conda")

# use miniconda
set(ENV{PATH} "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin:$ENV{PATH}")
set(SEARCH_PATHS
    ${CMAKE_CURRENT_LIST_DIR}/miniconda
    ${CMAKE_CURRENT_LIST_DIR}/miniconda/bin)

#------------------------------------------------------------------------------#
#   local `conda`
#------------------------------------------------------------------------------#
find_program(CONDA_EXE
    NAMES           conda
    PATHS           ${SEARCH_PATHS}
    HINTS           ${SEARCH_PATHS}
    PATH_SUFFIXES   bin
    NO_DEFAULT_PATH)

if("${CONDA_EXE}" STREQUAL "${DEFAULT_CONDA_EXE}")
    set(CONDA_EXE ${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/conda CACHE
        FILEPATH "Local conda installation" FORCE)
endif()

if(NOT CONDA_EXE OR NOT EXISTS "${CONDA_EXE}")
    message(FATAL_ERROR "Error! Could not find 'conda' command in \"${SEARCH_PATHS}\"")
endif()

get_filename_component(CONDA_DIR "${CONDA_EXE}" PATH)
set(SEARCH_PATHS ${CONDA_DIR} ${SEARCH_PATHS})

#------------------------------------------------------------------------------#
#   local `python`
#------------------------------------------------------------------------------#
find_program(PYTHON_EXE
    NAMES           python
    PATHS           ${SEARCH_PATHS}
    HINTS           ${SEARCH_PATHS}
    PATH_SUFFIXES   bin
    NO_DEFAULT_PATH)

if(NOT PYTHON_EXE OR NOT EXISTS "${PYTHON_EXE}")
    message(FATAL_ERROR "Error! Could not find 'python' command in \"${SEARCH_PATHS}\"")
endif()

#------------------------------------------------------------------------------#
#   setup
#------------------------------------------------------------------------------#
if(CONDA_PACKAGE_INSTALL)
    execute_process(COMMAND ${CONDA_EXE} config --add channels conda-forge)
    execute_process(COMMAND ${CONDA_EXE} config --add channels jrmadsen)
    execute_process(COMMAND ${CONDA_EXE} config --set always_yes yes --set changeps1 no)
    execute_process(COMMAND ${CONDA_EXE} update conda)
    #execute_process(COMMAND ${CONDA_EXE} install anaconda-client)
    execute_process(COMMAND ${CONDA_EXE} install python=${PYTHON_VERSION} ${CONDA_PKGS})
    # update matplotlib and freetype due to MacOS issues with default
    execute_process(COMMAND ${CONDA_EXE} update matplotlib)
    execute_process(COMMAND ${CONDA_EXE} update freetype)
    #execute_process(COMMAND ${CONDA_EXE} info -a)
    #execute_process(COMMAND ${CONDA_EXE} list --explicit)
endif()
