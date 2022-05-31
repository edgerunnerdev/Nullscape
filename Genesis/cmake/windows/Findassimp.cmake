set(ASSIMP_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/assimp-5.2.4/include" CACHE STRING "assimp - include directory")
set(ASSIMP_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/assimp-5.2.4/lib/win64/$<IF:$<CONFIG:Debug>,Debug,Release>" CACHE STRING "assimp - library directory")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(assimp REQUIRED_VARS ASSIMP_INCLUDE_DIRS ASSIMP_LIBRARY_DIRS)