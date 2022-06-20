set(IMGUIFILEBROWSER_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/ext/imgui-filebrowser/src/imgui-filebrowser")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(imgui-filebrowser REQUIRED_VARS IMGUIFILEBROWSER_INCLUDE_DIRS)