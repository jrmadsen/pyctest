
execute_process(COMMAND
    python -c "import sys; print('{}.{}'.format(sys.version_info[0], sys.version_info[1]))"
    OUTPUT_VARIABLE SYSTEM_PYTHON_VERSION)

if(NOT DEFINED USE_CONDA)
    set(USE_CONDA OFF CACHE BOOL "Enable conda build")
endif()
if(NOT DEFINED CONDA_QUICK)
    set(CONDA_QUICK OFF CACHE BOOL "Disable testing")
endif()
if(NOT DEFINED PYTHON_VERSION)
    set(PYTHON_VERSION ${SYSTEM_PYTHON_VERSION} CACHE STRING "Python version")
endif()

message(STATUS "USE_CONDA : ${USE_CONDA}")
message(STATUS "CONDA_QUICK : ${CONDA_QUICK}")
message(STATUS "PYTHON_VERSION : ${PYTHON_VERSION}")

execute_process(COMMAND
    python -c "import platform; print('{} {} {}'.format(platform.system(), platform.release(), platform.machine()))"
    OUTPUT_VARIABLE CTEST_BUILD_NAME)

set(CTEST_BUILD_NAME "${CTEST_BUILD_NAME} - Python ${PYTHON_VERSION}")
if(USE_CONDA)
    set(CTEST_BUILD_NAME "${CTEST_BUILD_NAME} [conda]")
endif()

set(CTEST_SOURCE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
set(CTEST_BINARY_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
# set(CTEST_CHECKOUT_COMMAND "")
set(CTEST_UPDATE_COMMAND "git")
#set(CTEST_CONFIGURE_COMMAND "")
#set(CTEST_COVERAGE_COMMAND "")
# set(CTEST_MEMORYCHECK_COMMAND "")
# set(CTEST_GENERATOR "")
# set(CTEST_GENERATOR_PLATFORM "")
set(CTEST_TIMEOUT "7200")
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_ERRORS "200")
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS "300")
set(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE "104857600")
set(BUILD_TYPE "MinSizeRel")
set(CTEST_UPDATE_VERSION_ONLY ON)
set(CTEST_UPDATE_TYPE "git")
set(CTEST_GIT_COMMAND "git")
set(CTEST_MODEL "Continuous")
if(EXISTS "$ENV{HOME}/.tokens/nersc-cdash")
    set(CTEST_TOKEN_FILE "$ENV{HOME}/.tokens/nersc-cdash")
endif()
set(ENV{LC_MESSAGES}        "en_EN" )

if(USE_CONDA)
    if(CONDA_QUICK)
        set(CTEST_BUILD_COMMAND "conda-build --no-test -c conda-forge -c jrmadsen --override-channels --python=${PYTHON_VERSION} ${CMAKE_CURRENT_LIST_DIR}")
    else()
        set(CTEST_BUILD_COMMAND "conda-build -c conda-forge -c jrmadsen --override-channels --python=${PYTHON_VERSION} ${CMAKE_CURRENT_LIST_DIR}")
    endif()
else()
    set(CTEST_BUILD_COMMAND "python setup.py install")
endif()

message(STATUS "BUILD_COMMAND : ${CTEST_BUILD_COMMAND}")
