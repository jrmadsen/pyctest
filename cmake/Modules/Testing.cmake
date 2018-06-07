
## -- CTest Config
if(EXISTS "${CMAKE_SOURCE_DIR}/CTestConfig.cmake")
    configure_file(${CMAKE_SOURCE_DIR}/CTestConfig.cmake
        ${CMAKE_BINARY_DIR}/CTestConfig.cmake @ONLY)
endif(EXISTS "${CMAKE_SOURCE_DIR}/CTestConfig.cmake")

# testing
ENABLE_TESTING()
if(PYCTEST_BUILD_TESTING)
    include(CTest)
endif(PYCTEST_BUILD_TESTING)


# ------------------------------------------------------------------------ #
# -- Function to create a temporary directory
# ------------------------------------------------------------------------ #
function(GET_TEMPORARY_DIRECTORY DIR_VAR DIR_MODEL)
    # create a root working directory
    if(WIN32)
        set(_TMP_DIR "$ENV{TEMP}")
        STRING(REPLACE "\\" "/" _TMP_DIR "${_TMP_DIR}")
        set(_TMP_ROOT "${_TMP_DIR}/${PROJECT_NAME}/cdash/${DIR_MODEL}")
    else(WIN32)
        set(_TMP_ROOT "/tmp/${PROJECT_NAME}/cdash/${DIR_MODEL}")
    endif(WIN32)
    set(${DIR_VAR} "${_TMP_ROOT}" PARENT_SCOPE)
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${_TMP_ROOT}) 
endfunction()


# ------------------------------------------------------------------------ #
# -- Configure Branch label
# ------------------------------------------------------------------------ #
if(PYCTEST_BUILD_TESTING)

    find_package(Git REQUIRED)

    execute_process(COMMAND ${GIT_EXECUTABLE} name-rev --name-only HEAD
        OUTPUT_VARIABLE CMAKE_SOURCE_BRANCH
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE "~[0-9]+" "" CMAKE_SOURCE_BRANCH "${CMAKE_SOURCE_BRANCH}")
    string(REGEX REPLACE "tags/" "" CMAKE_SOURCE_BRANCH "${CMAKE_SOURCE_BRANCH}")

    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --verify HEAD
        OUTPUT_VARIABLE CMAKE_SOURCE_REVISION
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_STRIP_TRAILING_WHITESPACE)

endif(PYCTEST_BUILD_TESTING)


# ------------------------------------------------------------------------ #
# -- Add options
# ------------------------------------------------------------------------ #
macro(add_ctest_options VARIABLE )

    get_cmake_property(_vars CACHE_VARIABLES)
    get_cmake_property(_nvars VARIABLES)
    foreach(_var ${_nvars})
        list(APPEND _vars ${_var})
    endforeach(_var ${_nvars})

    list(REMOVE_DUPLICATES _vars)
    list(SORT _vars)
    set(_set_vars ${ARGN})
    foreach(_var ${_vars})
        STRING(REGEX MATCH "^PYCTEST_USE_" _use_found "${_var}")
        STRING(REGEX MATCH ".*(_ROOT|_LIBRARY|_INCLUDE_DIR|_EXECUTABLE)$"
            _root_found "${_var}")
        STRING(REGEX MATCH "^(PREVIOUS_|CMAKE_|OSX_|DEFAULT_|EXTERNAL_|_|CTEST_|DOXYGEN_|QT_)"
            _skip_prefix "${_var}")
        STRING(REGEX MATCH ".*(_AVAILABLE|_LIBRARIES|_INCLUDE_DIRS)$"
            _skip_suffix "${_var}")

        if(_skip_prefix OR _skip_suffix)
            continue()
        endif(_skip_prefix OR _skip_suffix)

        if(_use_found OR _root_found)
            list(APPEND _set_vars ${_var})
        endif(_use_found OR _root_found)
    endforeach(_var ${_vars})

    list(REMOVE_DUPLICATES _set_vars)
    list(SORT _set_vars)
    foreach(_var ${_set_vars})
        if(NOT "${${_var}}" STREQUAL "" AND
            NOT "${${_var}}" STREQUAL "${_var}-NOTFOUND")
            add(${VARIABLE} "-D${_var}='${${_var}}'")
        endif(NOT "${${_var}}" STREQUAL "" AND
            NOT "${${_var}}" STREQUAL "${_var}-NOTFOUND")
    endforeach(_var ${_set_vars})

    unset(_vars)
    unset(_nvars)
    unset(_set_vars)

endmacro(add_ctest_options VARIABLE )


# ------------------------------------------------------------------------ #
# -- Configure CTest for CDash
# ------------------------------------------------------------------------ #
if(NOT PYCTEST_DASHBOARD_MODE AND PYCTEST_BUILD_TESTING)

    # get temporary directory for dashboard testing
    if(NOT DEFINED CMAKE_DASHBOARD_ROOT)
        GET_TEMPORARY_DIRECTORY(CMAKE_DASHBOARD_ROOT ${CTEST_MODEL})
    endif(NOT DEFINED CMAKE_DASHBOARD_ROOT)
    
    # set the CMake configure options
    add_ctest_options(CMAKE_CONFIGURE_OPTIONS
        CMAKE_BUILD_TYPE
        CMAKE_C_COMPILER CMAKE_CXX_COMPILER
        MPI_C_COMPILER MPI_CXX_COMPILER
        CTEST_MODEL CTEST_SITE
        PYCTEST_EXCEPTIONS
        BUILD_SHARED_LIBS
        PYCTEST_BUILD_EXAMPLES
        PYCTEST_TEST_MPI)

    set(cdash_templates Init Build Test Submit Glob Stages)
    if(USE_COVERAGE)
        list(APPEND cdash_templates Coverage)
    endif(USE_COVERAGE)
    if(MEMORYCHECK_COMMAND)
        list(APPEND cdash_templates MemCheck)
    endif(MEMORYCHECK_COMMAND)

    foreach(_type ${cdash_templates})
        ## -- CTest Setup
        if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/cdash/${_type}.cmake.in")
            configure_file(${CMAKE_SOURCE_DIR}/cmake/Templates/cdash/${_type}.cmake.in
                ${CMAKE_BINARY_DIR}/cdash/${_type}.cmake @ONLY)
        endif(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/cdash/${_type}.cmake.in")
    endforeach(_type Init Build Test Coverage MemCheck Submit Glob Stages)

    ## -- CTest Custom
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in")
        configure_file(${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in
            ${CMAKE_BINARY_DIR}/CTestCustom.cmake @ONLY)
    endif(EXISTS "${CMAKE_SOURCE_DIR}/cmake/Templates/CTestCustom.cmake.in")

endif(NOT PYCTEST_DASHBOARD_MODE AND PYCTEST_BUILD_TESTING)
