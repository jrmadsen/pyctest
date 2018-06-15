
################################################################################
#
#        PYCTEST Options
#
################################################################################

include(MacroUtilities)

add_option(PYCTEST_SETUP_PY "Configuration from setup.py file" OFF)
add_option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
# CMake options
add_option(CMAKE_CXX_STANDARD_REQUIRED "Require C++ standard" ON)
add_option(CMAKE_CXX_EXTENSIONS "Build with CXX extensions (e.g. gnu++11)" OFF)
# features
add_feature(CMAKE_BUILD_TYPE "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)")
add_feature(CMAKE_INSTALL_PREFIX "Installation prefix")
add_feature(CMAKE_C_FLAGS "C compiler flags")
add_feature(CMAKE_CXX_FLAGS "C++ compiler flags")
add_feature(CMAKE_CXX_STANDARD "C++ STL standard")

if(NOT PYCTEST_SETUP_PY)
    message(AUTHOR_WARNING
        "\nInstallation outside of \"python setup.py install\" is not recommended\n")
endif(NOT PYCTEST_SETUP_PY)
