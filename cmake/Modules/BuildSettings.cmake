################################################################################
#
#        Handles the build settings
#
################################################################################

include(GNUInstallDirs)

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(WIN32)
    set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ STL standard")
else(WIN32)
    set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ STL standard")
endif(WIN32)

if(GOOD_CMAKE)
    set(CMAKE_INSTALL_MESSAGE LAZY)
endif(GOOD_CMAKE)

# ensure only C++11, C++14, or C++17
if(NOT "${CMAKE_CXX_STANDARD}" STREQUAL "11" AND
    NOT "${CMAKE_CXX_STANDARD}" STREQUAL "14" AND
    NOT "${CMAKE_CXX_STANDARD}" STREQUAL "17" AND
    NOT "${CMAKE_CXX_STANDARD}" STREQUAL "1y" AND
    NOT "${CMAKE_CXX_STANDARD}" STREQUAL "1z")

    if(WIN32)
        set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ STL standard" FORCE)
    else(WIN32)
        set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ STL standard" FORCE)
    endif(WIN32)

endif(NOT "${CMAKE_CXX_STANDARD}" STREQUAL "11" AND
    NOT "${CMAKE_CXX_STANDARD}" STREQUAL "14" AND
    NOT "${CMAKE_CXX_STANDARD}" STREQUAL "17" AND
    NOT "${CMAKE_CXX_STANDARD}" STREQUAL "1y" AND
    NOT "${CMAKE_CXX_STANDARD}" STREQUAL "1z")

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

############
#   other
############
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
#    add_definitions(-DDEBUG)
else("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DNDEBUG)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
