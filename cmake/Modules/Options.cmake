
################################################################################
#
#        PYCTEST Options
#
################################################################################

include(MacroUtilities)

add_option(PYCTEST_BUILD_TESTING "Build testing for dashboard" OFF NO_FEATURE)
add_option(BUILD_SHARED_LIBS "Build shared libraries" ON)
# CMake options
add_option(CMAKE_CXX_STANDARD_REQUIRED "Require C++ standard" ON)
add_option(CMAKE_CXX_EXTENSIONS "Build with CXX extensions (e.g. gnu++11)" OFF)
# features
add_feature(PYCTEST_INSTALL_PREFIX "PYCTEST installation")
add_feature(CMAKE_BUILD_TYPE "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)")
add_feature(CMAKE_INSTALL_PREFIX "Installation prefix")
add_feature(CMAKE_C_FLAGS "C compiler flags")
add_feature(CMAKE_CXX_FLAGS "C++ compiler flags")
add_feature(CMAKE_CXX_STANDARD "C++11 STL standard")

mark_as_advanced(PYCTEST_BUILD_TESTING)

set(CTEST_SITE "${HOSTNAME}" CACHE STRING "CDash submission site")
set(CTEST_MODEL "Continuous" CACHE STRING "CDash submission track")
if(PYCTEST_BUILD_TESTING)
    # if this is directory we are running CDash (don't set to ON)
    add_option(PYCTEST_DASHBOARD_MODE
        "Internally used to skip generation of CDash files" OFF NO_FEATURE)
    mark_as_advanced(PYCTEST_DASHBOARD_MODE)
    add_feature(CTEST_MODEL "CDash submission track")
    add_feature(CTEST_SITE "CDash submission site")

    if(NOT PYCTEST_DASHBOARD_MODE)
        add_option(CTEST_LOCAL_CHECKOUT
            "Use the local source tree for CTest/CDash" OFF NO_FEATURE)
    endif(NOT PYCTEST_DASHBOARD_MODE)
endif(PYCTEST_BUILD_TESTING)
