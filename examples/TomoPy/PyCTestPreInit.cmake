
message(STATUS "Including ${CMAKE_CURRENT_LIST_FILE}...")

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

find_program(WGET_CMD NAMES wget)
find_program(BASH_CMD NAMES bash)

set(CONDA_PKGS nose six numpy h5py scipy scikit-image pywavelets mkl-devel
    mkl_fft python-coveralls dxchange numexpr pip)

# if not already installed
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/python")
    if("${PYTHON_VERSION}" VERSION_EQUAL "2.7")
        set(MINICONDA_VER "Miniconda")
        list(APPEND CONDA_PKGS "futures")
    else()
        set(MINICONDA_VER "Miniconda3")
    endif()

    if(APPLE)
        set(_OS "MacOSX")
    elif(WIN32)
        set(_OS "Windows")
    else()
        set(_OS "Linux")
    endif(APPLE)

    set(MINICONDA_URL "https://repo.continuum.io/miniconda/${MINICONDA_VER}-latest-${_OS}-x86_64.sh")
    execute_process(COMMAND
        ${WGET_CMD} ${MINICONDA_URL} -O ${CMAKE_CURRENT_LIST_DIR}/miniconda.sh)
    execute_process(COMMAND
        ${BASH_CMD} ${CMAKE_CURRENT_LIST_DIR}/miniconda.sh -b -p ${CMAKE_CURRENT_LIST_DIR}/miniconda)

endif(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin/python")

# use miniconda
set(ENV{PATH} "${CMAKE_CURRENT_LIST_DIR}/miniconda/bin:$ENV{PATH}")
find_program(CONDA_CMD NAMES conda
    HINTS ${CMAKE_CURRENT_LIST_DIR}/miniconda
    PATH_SUFFIXES bin)

execute_process(COMMAND ${CONDA_CMD} config --add channels conda-forge)
execute_process(COMMAND ${CONDA_CMD} config --set always_yes yes --set changeps1 no)
execute_process(COMMAND ${CONDA_CMD} update conda)
execute_process(COMMAND ${CONDA_CMD} install anaconda-client)
execute_process(COMMAND ${CONDA_CMD} install python=${PYTHON_VERSION} ${CONDA_PKGS})
execute_process(COMMAND ${CONDA_CMD} info -a)
execute_process(COMMAND ${CONDA_CMD} list --explicit)
