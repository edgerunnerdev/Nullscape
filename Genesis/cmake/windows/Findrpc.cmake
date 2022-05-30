set(RPC_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/rpclib-2.3.0/include" CACHE STRING "RPC - include directory")
set(RPC_LIBRARY_DIRS "${CMAKE_SOURCE_DIR}/Genesis/libs/rpclib-2.3.0/lib/win64/$<IF:$<CONFIG:Debug>,Debug,Release>" CACHE STRING "RPC - library directory")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(rpc REQUIRED_VARS RPC_INCLUDE_DIRS RPC_LIBRARY_DIRS)