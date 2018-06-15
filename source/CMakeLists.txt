
# needed by CMake build
set(CMAKE_ROOT ${CMAKE_CURRENT_LIST_DIR}/kitware-cmake CACHE
    PATH "CMake root directory" FORCE)

# needed by CMake build
set(CMake_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/kitware-cmake)

# Compute CMake_VERSION, etc.
include(${CMAKE_CURRENT_LIST_DIR}/kitware-cmake/Source/CMakeVersionCompute.cmake)

# needed by CMake build
set(CMAKE_DATA_DIR
    share/cmake-${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}
    CACHE PATH "CMake data installation directory" FORCE)

#------------------------------------------------------------------------------#
# only define if less than 3.10.0
#
if(CMAKE_VERSION VERSION_LESS "3.10.0")
macro(include_guard)

    if("${ARGN}" STREQUAL "GLOBAL")
        set(_GLOBAL ON)
    elseif("${ARGN}" STREQUAL "DIRECTORY")
        set(_DIRECTORY ON)
    else()
        set(_VARIABLE ON)
    endif()

    set(_VARNAME __${CMAKE_CURRENT_LIST_FILE}_VAR__)

    if(_GLOBAL)
        get_property(_PROP
            GLOBAL
            PROPERTY ${_VARNAME}
            SET
        )
        if(_PROP)
            return()
        endif()
        set_property(GLOBAL PROPERTY ${_VARNAME} TRUE)

    elseif(_DIRECTORY)
        get_property(_PROP
            DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            PROPERTY ${_VARNAME}
            SET
        )

        if(_PROP)
            return()
        endif()

        set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            PROPERTY ${_VARNAME} TRUE)

    elseif(_VARIABLE)
        get_property(_PROP
            VARIABLE
            PROPERTY ${_VARNAME}
            SET
        )

        if(_PROP)
            return()
        endif()

        set_property(PROPERTY ${_VARNAME} TRUE)
    endif()

endmacro(include_guard)
endif(CMAKE_VERSION VERSION_LESS "3.10.0")
#------------------------------------------------------------------------------#

# development installation components
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME development)
add_subdirectory(kitware-cmake)

# python installation components
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME python)
add_subdirectory(python)

# set back to ignore
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME ignore)

# copy to build directory
execute_process(COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_LIST_DIR}/kitware-cmake/Modules
    ${CMAKE_BINARY_DIR}/pyctest/share/cmake-${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}/Modules)
