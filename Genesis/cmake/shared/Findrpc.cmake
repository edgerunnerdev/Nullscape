set(RPC_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/ext/rpclib/include" CACHE STRING "RPC - include directory")
set(RPC_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/Genesis/ext/rpclib/lib" CACHE STRING "RPC - library directory")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(rpc REQUIRED_VARS RPC_INCLUDE_DIRS RPC_LIBRARY_DIRS)