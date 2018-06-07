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

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(PYCTEST_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "pyctest installation prefix")

add_feature(CMAKE_C_FLAGS_${_CONFIG} "C compiler build type flags")
add_feature(CMAKE_CXX_FLAGS_${_CONFIG} "C++ compiler build type flags")

################################################################################
#
#   Non-python installation directories
#
################################################################################

if(PYCTEST_DEVELOPER_INSTALL)

    set(PYCTEST_INSTALL_DATAROOTDIR ${CMAKE_INSTALL_DATAROOTDIR})
    if(NOT IS_ABSOLUTE ${PYCTEST_INSTALL_DATAROOTDIR})
        set(PYCTEST_INSTALL_DATAROOTDIR "${PYCTEST_INSTALL_PREFIX}/share"
            CACHE PATH "Installation root directory for data" FORCE)
    endif()

    set(PYCTEST_INSTALL_CMAKEDIR ${PYCTEST_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME}
        CACHE PATH "Installation for CMake config" FORCE)
    set(PYCTEST_INSTALL_INCLUDEDIR ${PYCTEST_INSTALL_PREFIX}/include
        CACHE PATH "Installation for include directories" FORCE)
    set(PYCTEST_INSTALL_LIBDIR ${PYCTEST_INSTALL_PREFIX}/${LIBDIR_DEFAULT}
        CACHE PATH "Installation for libraries" FORCE)
    set(PYCTEST_INSTALL_BINDIR ${PYCTEST_INSTALL_PREFIX}/bin
        CACHE PATH "Installation for executables" FORCE)
    set(PYCTEST_INSTALL_MANDIR ${PYCTEST_INSTALL_DATAROOTDIR}/man
        CACHE PATH "Installation for executables" FORCE)
    set(PYCTEST_INSTALL_DOCDIR ${PYCTEST_INSTALL_DATAROOTDIR}/doc
        CACHE PATH "Installation for executables" FORCE)

else(PYCTEST_DEVELOPER_INSTALL)

    # cmake installation folder
    set(PYCTEST_INSTALL_CMAKEDIR  ${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME}
        CACHE PATH "Installation directory for CMake package config files")
    # the rest of the installation folders
    foreach(_TYPE in DATAROOT INCLUDE LIB BIN MAN DOC)
        set(PYCTEST_INSTALL_${_TYPE}DIR ${CMAKE_INSTALL_${_TYPE}DIR})
    endforeach(_TYPE in DATAROOT INCLUDE LIB BIN MAN DOC)

endif(PYCTEST_DEVELOPER_INSTALL)

# create the full path version and generic path versions
foreach(_TYPE in DATAROOT CMAKE INCLUDE LIB BIN MAN DOC)
    # set the absolute versions
    if(NOT IS_ABSOLUTE "${PYCTEST_INSTALL_${_TYPE}DIR}")
        set(PYCTEST_INSTALL_FULL_${_TYPE}DIR ${CMAKE_INSTALL_PREFIX}/${PYCTEST_INSTALL_${_TYPE}DIR})
    else(NOT IS_ABSOLUTE "${PYCTEST_INSTALL_${_TYPE}DIR}")
        set(PYCTEST_INSTALL_FULL_${_TYPE}DIR ${PYCTEST_INSTALL_${_TYPE}DIR})
    endif(NOT IS_ABSOLUTE "${PYCTEST_INSTALL_${_TYPE}DIR}")

    # generic "PROJECT_INSTALL_" variables (used by documentation)"
    set(PROJECT_INSTALL_${_TYPE}DIR ${PYCTEST_INSTALL_${TYPE}DIR})
    set(PROJECT_INSTALL_FULL_${_TYPE}DIR ${PYCTEST_INSTALL_FULL_${TYPE}DIR})

endforeach(_TYPE in DATAROOT CMAKE INCLUDE LIB BIN MAN DOC)
