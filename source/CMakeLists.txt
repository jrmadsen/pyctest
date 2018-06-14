
set(CMAKE_ROOT ${CMAKE_CURRENT_LIST_DIR}/kitware-cmake CACHE
    PATH "CMake root directory" FORCE)

set(CMake_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/kitware-cmake)

# Compute CMake_VERSION, etc.
include(${CMAKE_CURRENT_LIST_DIR}/kitware-cmake/Source/CMakeVersionCompute.cmake)

set(CMAKE_DATA_DIR
    share/cmake-${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}
    CACHE PATH "CMake data installation directory" FORCE)

if(APPLE)
    # conda build on macOS fails during post-build because it tries to use
    # otool on these bc of execute permissions
    find_program(CHMOD_EXE NAMES chmod)
    file(GLOB MODULE_TEMPLATES ${CMAKE_CURRENT_LIST_DIR}/kitware-cmake/Modules/CPack.*.in)
    execute_process(COMMAND
        ${CHMOD_EXE} a-x ${MODULE_TEMPLATES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif(APPLE)

add_subdirectory(kitware-cmake)
add_subdirectory(python)

execute_process(COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_LIST_DIR}/kitware-cmake/Modules
    ${CMAKE_BINARY_DIR}/share/cmake-${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}/Modules)
