################################################################################
#
#        Set up the package installation
#
################################################################################

## -- CTest Config
if(EXISTS "${CMAKE_SOURCE_DIR}/CTestConfig.cmake")
    configure_file(${CMAKE_SOURCE_DIR}/CTestConfig.cmake
        ${CMAKE_BINARY_DIR}/CTestConfig.cmake @ONLY)
endif(EXISTS "${CMAKE_SOURCE_DIR}/CTestConfig.cmake")

set(_SOURCE ${CMAKE_SOURCE_DIR}/cmake/Templates/cdash)
set(_BINARY ${CMAKE_BINARY_DIR}/pyctest/cdash)
set(cdash_templates Init Build Test Submit Glob Stages Coverage MemCheck Utilities)

foreach(_type ${cdash_templates})
    ## -- CTest Setup
    if(EXISTS "${_SOURCE}/${_type}.cmake.in")
        configure_file(${_SOURCE}/${_type}.cmake.in ${_BINARY}/${_type}.cmake @ONLY)
        install(FILES ${_BINARY}/${_type}.cmake
            DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/cdash
            COMPONENT development)
    endif(EXISTS "${_SOURCE}/${_type}.cmake.in")
endforeach(_type Init Build Test Coverage MemCheck Submit Glob Stages)

## -- CTest Custom
if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in")
    configure_file(${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in
        ${CMAKE_BINARY_DIR}/CTestCustom.cmake @ONLY)
endif(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in")

# testing
#ENABLE_TESTING()
#include(CTest)

