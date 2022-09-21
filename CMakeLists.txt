cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW) # Enable CMAKE_MSVC_RUNTIME_LIBRARY.

# Setup vcpkg script with CMake (note: should be placed before project() call)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
list(APPEND VCPKG_FEATURE_FLAGS "versions")
if(WIN32)
  set(VCPKG_TARGET_TRIPLET "x64-windows-static" CACHE STRING "")
endif()

project(Nullscape)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Genesis/cmake/shared)
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

# vcpkg dependencies
find_package(CGAL CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(rpclib CONFIG REQUIRED)

add_subdirectory("Genesis")
add_subdirectory("Game")