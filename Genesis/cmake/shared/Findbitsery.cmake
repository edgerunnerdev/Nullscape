set(BITSERY_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/ext/bitsery/src/bitsery/include")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(bitsery REQUIRED_VARS BITSERY_INCLUDE_DIRS)