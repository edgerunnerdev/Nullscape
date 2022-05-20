set(VPX_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/libvpx-v1.11.0/include" CACHE STRING "VPX - include directory")
set(VPX_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/libvpx-v1.11.0/lib/x64/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:Release>" CACHE STRING "VPX - library directory")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VPX REQUIRED_VARS VPX_INCLUDE_DIRS VPX_LIBRARY_DIRS)