set(XXHASH_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/ext/xxhash/src/xxhash")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(xxhash REQUIRED_VARS XXHASH_INCLUDE_DIRS)