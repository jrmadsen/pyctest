
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
