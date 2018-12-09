

macro(SET_IF_DEFINED IF_DEF_VAR PREFIX_VAR SET_VAR)
    if(DEFINED "${IF_DEF_VAR}")
        set(${SET_VAR} "${PREFIX_VAR} \"${${IF_DEF_VAR}}\"")
    endif()
endmacro()

include(ProcessorCount)
ProcessorCount(CTEST_PROCESSOR_COUNT)

cmake_policy(SET CMP0009 NEW)
cmake_policy(SET CMP0011 NEW)

# ---------------------------------------------------------------------------- #
# -- Check to see if there is a ctest token (for CDash submission)
# ---------------------------------------------------------------------------- #
macro(CHECK_FOR_CTEST_TOKEN)

    # set using token to off
    set(CTEST_USE_TOKEN OFF)
    # set token to empty
    set(CTEST_TOKEN     "")

    if(NOT "${CTEST_TOKEN_FILE}" STREQUAL "")
        string(REGEX REPLACE "^~" "$ENV{HOME}" CTEST_TOKEN_FILE "${CTEST_TOKEN_FILE}")
    endif(NOT "${CTEST_TOKEN_FILE}" STREQUAL "")

    # check for a file containing token
    if(NOT "${CTEST_TOKEN_FILE}" STREQUAL "" AND EXISTS "${CTEST_TOKEN_FILE}")
        message(STATUS "Reading CTest token file: ${CTEST_TOKEN_FILE}")
        file(READ "${CTEST_TOKEN_FILE}" CTEST_TOKEN)
        string(REPLACE "\n" "" CTEST_TOKEN "${CTEST_TOKEN}")
    endif(NOT "${CTEST_TOKEN_FILE}" STREQUAL "" AND EXISTS "${CTEST_TOKEN_FILE}")

    # if no file, check the environment
    if("${CTEST_TOKEN}" STREQUAL "" AND NOT "$ENV{CTEST_TOKEN}" STREQUAL "")
        set(CTEST_TOKEN "$ENV{CTEST_TOKEN}")
    endif("${CTEST_TOKEN}" STREQUAL "" AND NOT "$ENV{CTEST_TOKEN}" STREQUAL "")

    # if non-empty token, set CTEST_USE_TOKEN to ON
    if(NOT "${CTEST_TOKEN}" STREQUAL "")
        set(CTEST_USE_TOKEN ON)
    endif(NOT "${CTEST_TOKEN}" STREQUAL "")

endmacro(CHECK_FOR_CTEST_TOKEN)

#-------------------------------------------------------------------------#
# Some programs
#
find_program(CTEST_CMAKE_COMMAND    NAMES cmake)
find_program(CTEST_GIT_COMMAND      NAMES git)

set(CONFIG_FILE_PATH "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(CONFIG_FILE_PATH "${CONFIG_FILE_PATH}" PATH)
get_filename_component(CONFIG_FILE_PATH "${CONFIG_FILE_PATH}" PATH)
ctest_read_custom_files("${CONFIG_FILE_PATH}")

#-------------------------------------------------------------------------#
# Standard
#
ctest_start(${CTEST_MODEL} TRACK ${CTEST_MODEL}
    ${CTEST_SOURCE_DIRECTORY} ${CTEST_BINARY_DIRECTORY})
ctest_update(SOURCE "${CTEST_SOURCE_DIRECTORY}" RETURN_VALUE up_ret)
ctest_build(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE build_ret)

check_for_ctest_token()
if(CTEST_USE_TOKEN) # submit with token
    ctest_submit(RETURN_VALUE ret HTTPHEADER "Authorization: Bearer ${CTEST_TOKEN}")
endif(CTEST_USE_TOKEN)
