################################################################################
#
# Project settings
#
################################################################################

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
    set(CMAKE_BUILD_TYPE Release)
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" _CONFIG)

if(WIN32)
    set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ STL standard")
else(WIN32)
    set(CMAKE_CXX_STANDARD 11 CACHE STRING "C++ STL standard")
endif(WIN32)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_feature(CMAKE_C_FLAGS_${_CONFIG} "C compiler build type flags")
add_feature(CMAKE_CXX_FLAGS_${_CONFIG} "C++ compiler build type flags")

include(GNUInstallDirs)

# create the full path version and generic path versions
foreach(_TYPE in DATAROOT INCLUDE LIB BIN MAN DOC)
    # set the absolute versions
    if(NOT IS_ABSOLUTE "${CMAKE_INSTALL_FULL_${_TYPE}DIR}")
        set(CMAKE_INSTALL_FULL_${_TYPE}DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_${_TYPE}DIR})
    endif(NOT IS_ABSOLUTE "${CMAKE_INSTALL_FULL_${_TYPE}DIR}")
endforeach(_TYPE in DATAROOT CMAKE INCLUDE LIB BIN MAN DOC)
