################################################################################
#
#        Handles the build settings
#
################################################################################

if(WIN32)
    set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ STL standard")
else(WIN32)
    set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ STL standard")
endif(WIN32)

if(GOOD_CMAKE)
    set(CMAKE_INSTALL_MESSAGE LAZY)
endif(GOOD_CMAKE)

set(CXX_STANDARD_OPTIONS 11 14 17 1y 1z)
set(VALID_CXX_STANDARD OFF)

foreach(_STANDARD ${CXX_STANDARD_OPTIONS})
    if("${CMAKE_CXX_STANDARD}" STREQUAL "${_STANDARD}")
        set(VALID_CXX_STANDARD ON)
    endif("${CMAKE_CXX_STANDARD}" STREQUAL "${_STANDARD}")
endforeach(_STANDARD ${CXX_STANDARD_OPTIONS})

# ensure only C++11, C++14, or C++17
if(NOT VALID_CXX_STANDARD)
    if(WIN32)
        set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ STL standard" FORCE)
    else(WIN32)
        set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ STL standard" FORCE)
    endif(WIN32)

endif(NOT VALID_CXX_STANDARD)

# the CXX standard flag
if(CMAKE_CXX_COMPILER_IS_GNU)
    add(CMAKE_CXX_FLAGS "-std=c++${CMAKE_CXX_STANDARD}")
elseif(CMAKE_CXX_COMPILER_IS_CLANG)
    add(CMAKE_CXX_FLAGS "-std=c++${CMAKE_CXX_STANDARD} -stdlib=libc++")
elseif(CMAKE_CXX_COMPILER_IS_INTEL)
    add(CMAKE_CXX_FLAGS "-std=c++${CMAKE_CXX_STANDARD}")
elseif(CMAKE_CXX_COMPILER_IS_PGI)
    add(CMAKE_CXX_FLAGS "--c++${CMAKE_CXX_STANDARD} -A")
elseif(CMAKE_CXX_COMPILER_IS_XLC)
    if(CMAKE_CXX_STANDARD GREATER 11)
        add(CMAKE_CXX_FLAGS "-std=c++1y")
    else(CMAKE_CXX_STANDARD GREATER 11)
        add(CMAKE_CXX_FLAGS "-std=c++11")
    endif(CMAKE_CXX_STANDARD GREATER 11)
elseif(CMAKE_CXX_COMPILER_IS_MSVC)
    add(CMAKE_CXX_FLAGS "-std:c++${CMAKE_CXX_STANDARD}")
endif(CMAKE_CXX_COMPILER_IS_GNU)

# used by configure_package_*
set(LIBNAME pyctest)

# set the output directory (critical on Windows)
# if PYCTEST_OUTPUT_DIR is not defined, set to CMAKE_BINARY_DIR
if(NOT DEFINED PYCTEST_OUTPUT_DIR OR "${PYCTEST_OUTPUT_DIR}" STREQUAL "")
    set(PYCTEST_OUTPUT_DIR ${CMAKE_BINARY_DIR})
endif(NOT DEFINED PYCTEST_OUTPUT_DIR OR "${PYCTEST_OUTPUT_DIR}" STREQUAL "")

# set the CMAKE_RUNTIME_OUTPUT_DIRECTORY variable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PYCTEST_OUTPUT_DIR}/pyctest/bin)

# not debug definition
if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DNDEBUG)
endif(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:strictStrings-")
endif(WIN32)
