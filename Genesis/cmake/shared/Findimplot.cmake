set(IMPLOT_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/ext/implot/src/implot")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(implot REQUIRED_VARS IMPLOT_INCLUDE_DIRS)