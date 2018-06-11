
## -- CTest Config
if(EXISTS "${CMAKE_SOURCE_DIR}/CTestConfig.cmake")
    configure_file(${CMAKE_SOURCE_DIR}/CTestConfig.cmake
        ${CMAKE_BINARY_DIR}/CTestConfig.cmake @ONLY)
endif(EXISTS "${CMAKE_SOURCE_DIR}/CTestConfig.cmake")

set(cdash_templates Init Build Test Submit Glob Stages Coverage MemCheck)
foreach(_type ${cdash_templates})
    ## -- CTest Setup
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/cdash/${_type}.cmake.in")
        configure_file(${CMAKE_SOURCE_DIR}/cmake/Templates/cdash/${_type}.cmake.in
            ${CMAKE_BINARY_DIR}/cdash/${_type}.cmake @ONLY)
        install(FILES ${CMAKE_BINARY_DIR}/cdash/${_type}.cmake
            DESTINATION cdash
            COMPONENT development)
    endif(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/cdash/${_type}.cmake.in")
endforeach(_type Init Build Test Coverage MemCheck Submit Glob Stages)

## -- CTest Custom
if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in")
    configure_file(${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in
        ${CMAKE_BINARY_DIR}/CTestCustom.cmake @ONLY)
endif(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in")

# testing
ENABLE_TESTING()
include(CTest)

