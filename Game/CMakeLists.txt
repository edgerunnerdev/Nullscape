﻿cmake_minimum_required(VERSION 3.15)
project(Game)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h src/*.hpp src/*.rc src/*.inl)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src FILES ${SOURCE_FILES})

find_package(Bullet REQUIRED)
find_package(Genesis REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(MagicEnum REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(xxhash REQUIRED)

include_directories(
  src/ 
  ${GENESIS_INCLUDE_DIRS}
  ${GENESISCORE_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIRS}
  ${MAGICENUM_INCLUDE_DIRS}
  ${XXHASH_INCLUDE_DIRS}
)

link_directories(
  ${GENESIS_LIBRARY_DIRS}
  ${GENESISCORE_LIBRARY_DIRS}
  ${GLEW_LIBRARY_DIRS}
  ${RPC_LIBRARY_DIRS}
)

link_libraries(
  freetype
  Genesis
  GenesisCore
  rpclib::rpc
)

set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

if(WIN32)
    add_executable(Game WIN32 ${SOURCE_FILES})
    target_compile_definitions(Game PRIVATE UNICODE _UNICODE _HASEXCEPTIONS=0)
    target_link_options(Game PRIVATE $<$<CONFIG:Final>:/INCREMENTAL:NO /LTCG>)
    target_link_libraries(Game PRIVATE Opengl32 glu32 ws2_32)
    set_target_properties(Game PROPERTIES VS_STARTUP_PROJECT ${OUTPUT_DIRECTORY})
    set_target_properties(Game PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${OUTPUT_DIRECTORY})
	set_target_properties(Game PROPERTIES VS_GLOBAL_VcpkgEnableManifest true)	
else()
    add_executable(Game ${SOURCE_FILES})
    target_link_libraries(Game PRIVATE GL pthread)
endif()

target_compile_definitions(Game PRIVATE $<$<CONFIG:Final>:NDEBUG _FINAL>)

option(USE_STEAM "Use the Steam integration" ON)
if(USE_STEAM)
  message(STATUS "Using Steam integration.")
  find_package(steamworks REQUIRED)
  target_include_directories(Game PRIVATE ${STEAMWORKS_INCLUDE_DIRS})
  target_compile_definitions(Game PRIVATE USE_STEAM=1 STEAM_APP_ID=1123230)
  target_link_directories(Game PRIVATE ${STEAMWORKS_LIBRARY_DIRS})
  target_link_libraries(Game PRIVATE steam_api64)
endif()

set_target_properties(Game PROPERTIES OUTPUT_NAME "Hyperscape")
set_target_properties(Game PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY})
set_target_properties(Game PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY})
set_target_properties(Game PROPERTIES RUNTIME_OUTPUT_DIRECTORY_FINAL ${OUTPUT_DIRECTORY})

target_compile_options(Game PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall 
          -Werror
          -Wno-reorder-ctor
          -fstandalone-debug
          >
     $<$<CXX_COMPILER_ID:MSVC>:
          /MP # build with multiple processes
          /WX # warnings as errors
          /wd4100 # unreferenced formal parameter
          /wd4121 # alignment of a member was sensitive to packing
          /wd4127 # conditional expression is constant
          /wd4505 # unreferenced local function has been removed
          /wd4189 # local variable is initialized but not referenced
          /wd4201 # nonstandard extension used : nameless struct/union
          /wd4702 # unreachable code
          >
)

add_custom_target(BUILD_RESOURCES ALL DEPENDS alwaysrun)
add_custom_command(TARGET BUILD_RESOURCES POST_BUILD COMMAND ${CMAKE_CURRENT_LIST_DIR}/../Genesis/bin/Forge.exe -m standalone -a ${CMAKE_CURRENT_LIST_DIR}/bin/assets -c ${CMAKE_CURRENT_LIST_DIR}/../Genesis/bin/compilers -d ${CMAKE_CURRENT_LIST_DIR}/bin/data -i ${CMAKE_CURRENT_LIST_DIR}/bin/intermediates)
add_dependencies(BUILD_RESOURCES Forge GenericComp ModelComp ShaderComp)
add_dependencies(Game BUILD_RESOURCES)