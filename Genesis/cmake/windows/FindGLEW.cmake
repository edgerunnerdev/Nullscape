set(GLEW_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/glew-2.1.0/include" CACHE STRING "GLEW - include directory")
set(GLEW_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/glew-2.1.0/lib/Release/x64" CACHE STRING "GLEW - library directory")
set(GLEW_BINARY_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/glew-2.1.0/bin/Release/x64" CACHE STRING "GLEW - library directory")
set(GLEW_LIBRARIES glew32)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLEW REQUIRED_VARS GLEW_INCLUDE_DIRS GLEW_LIBRARY_DIRS GLEW_LIBRARIES)