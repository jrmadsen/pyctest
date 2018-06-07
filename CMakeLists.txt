cmake_minimum_required(VERSION 2.8.12)

################################################################################

if(APPLE)
    # force the Clang compiler on macOS
    if(NOT "${CMAKE_C_COMPILER}" STREQUAL "/usr/bin/clang")
        unset(CMAKE_C_COMPILER CACHE)
        set(CMAKE_C_COMPILER /usr/bin/clang CACHE FILEPATH "C compiler" FORCE)
        message(STATUS "Forcing C compiler: ${CMAKE_C_COMPILER}")
    endif(NOT "${CMAKE_C_COMPILER}" STREQUAL "/usr/bin/clang")
    if(NOT "${CMAKE_CXX_COMPILER}" STREQUAL "/usr/bin/clang++")
        unset(CMAKE_CXX_COMPILER CACHE)
        set(CMAKE_CXX_COMPILER /usr/bin/clang++ CACHE FILEPATH "C++ compiler" FORCE)
        message(STATUS "Forcing C++ compiler: ${CMAKE_CXX_COMPILER}")
    endif(NOT "${CMAKE_CXX_COMPILER}" STREQUAL "/usr/bin/clang++")
endif(APPLE)

#message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
#message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")

################################################################################
#
#   CMake settings
#
################################################################################

file(READ "${CMAKE_CURRENT_LIST_DIR}/VERSION" VERSION_STRING LIMIT_COUNT 1)
string(REGEX REPLACE "(\n|\r)" "" VERSION_STRING "${VERSION_STRING}")
string(REGEX REPLACE "[A-Za-z].*" "" VERSION_STRING "${VERSION_STRING}")
set(PYCTEST_VERSION "${VERSION_STRING}" CACHE STRING "Version of pyctest project" FORCE)

if(CMAKE_VERSION VERSION_LESS 3.1.3)
    set(GOOD_CMAKE OFF CACHE INTERNAL "CMake does not support CMAKE_CXX_STANDARD" FORCE)
else(CMAKE_VERSION VERSION_LESS 3.1.3)
    set(GOOD_CMAKE ON CACHE INTERNAL "CMake supports CMAKE_CXX_STANDARD" FORCE)
endif(CMAKE_VERSION VERSION_LESS 3.1.3)

if(GOOD_CMAKE)
    cmake_policy(SET CMP0048 NEW)
    cmake_policy(SET CMP0042 NEW)
    #cmake_policy(SET CMP0054 NEW)
    project(pyctest LANGUAGES C CXX VERSION ${PYCTEST_VERSION})
else(GOOD_CMAKE)
    project(pyctest C CXX)
    set(PROJECT_VERSION ${PYCTEST_VERSION})
    string(REPLACE "." ";" "${PROJECT_VERSION}" _VERSION_LIST)
    list(GET _VERSION_LIST 0 PROJECT_VERSION_MAJOR)
    list(GET _VERSION_LIST 1 PROJECT_VERSION_MINOR)
    list(GET _VERSION_LIST 2 PROJECT_VERSION_PATCH)
endif(GOOD_CMAKE)

message(STATUS "${PROJECT_NAME} version ${PYCTEST_VERSION}")

set(CMAKE_MODULE_PATH 
    ${CMAKE_CURRENT_LIST_DIR}/cmake/Modules 
    ${CMAKE_CURRENT_LIST_DIR}/source/kitware-cmake/Modules
    ${CMAKE_MODULE_PATH})

foreach(_TYPE MAJOR MINOR PATCH)
    set(PYCTEST_VERSION_${_TYPE} ${PROJECT_VERSION_${_TYPE}} CACHE STRING "${PROJECT_NAME} ${_TYPE} version" FORCE)
    mark_as_advanced(PYCTEST_VERSION_${_TYPE})
endforeach(_TYPE MAJOR MINOR PATCH)

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME development)
mark_as_advanced(PYCTEST_VERSION)

################################################################################

include(MacroUtilities)
include(Options)
include(Compilers)
include(GNUInstallDirs)
include(ProjectSettings)
include(BuildSettings)
include(Packages)

################################################################################
#   pyctest source
################################################################################

set(BUILD_CursesDialog OFF CACHE BOOL "Disable building curses dialog" FORCE)
set(BUILD_QtDialog OFF CACHE BOOL "Disable building Qt dialog" FORCE)
set(CTEST_TEST_CPACK OFF CACHE BOOL "Disable CTest testing of CPack" FORCE)
set(CTEST_TEST_CTEST OFF CACHE BOOL "Disable CTest testing of CTest" FORCE)
set(CMAKE_RUN_LONG_TESTS OFF CACHE BOOL "Disable CMake long tests" FORCE)

add_subdirectory(source)

################################################################################
#   Package setup
################################################################################

include(PackageConfigure)

################################################################################
#   Testing
################################################################################
enable_testing()
include(CTest)
include(Testing)
