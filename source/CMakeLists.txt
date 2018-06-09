
set(CMAKE_ROOT ${CMAKE_CURRENT_LIST_DIR}/kitware-cmake CACHE
    PATH "CMake root directory" FORCE)

set(CMake_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/kitware-cmake)

# Compute CMake_VERSION, etc.
include(${CMAKE_CURRENT_LIST_DIR}/kitware-cmake/Source/CMakeVersionCompute.cmake)

set(CMAKE_DATA_DIR
    share/cmake-${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}
    CACHE PATH "CMake data installation directory" FORCE)

add_subdirectory(kitware-cmake)
add_subdirectory(python)

execute_process(COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_LIST_DIR}/kitware-cmake/Modules
    ${CMAKE_BINARY_DIR}/share/cmake-${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}/Modules)
