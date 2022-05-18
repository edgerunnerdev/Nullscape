set(BULLET3_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/bullet3-2.87/include" CACHE STRING "Bullet3 - include directory")
set(BULLET3_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/bullet3-2.87/lib/win32" CACHE STRING "Bullet3 - library directory")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(bullet3 REQUIRED_VARS BULLET3_INCLUDE_DIRS BULLET3_LIBRARY_DIRS)