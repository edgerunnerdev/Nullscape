cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW) # Enable CMAKE_MSVC_RUNTIME_LIBRARY.
project(Hyperscape)

if(WIN32)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Genesis/cmake/windows)
else()
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Genesis/cmake/linux)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Final" CACHE STRING "Custom build types" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXE_LINKER_FLAGS_FINAL "")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory("Genesis")
add_subdirectory("Game")