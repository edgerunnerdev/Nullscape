set(MAGICENUM_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/ext/MagicEnum/src/MagicEnum/include")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MagicEnum REQUIRED_VARS MAGICENUM_INCLUDE_DIRS)