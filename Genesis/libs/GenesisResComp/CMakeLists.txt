﻿cmake_minimum_required(VERSION 3.15)
set(PROJECT_NAME "GenesisResComp")
project(${PROJECT_NAME})

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src FILES ${SOURCE_FILES})

find_package(CmdParser REQUIRED)
find_package(GenesisCore REQUIRED)

include_directories(
    src/
    ${CMDPARSER_INCLUDE_DIRS}
    ${GENESISCORE_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_HOST_SYSTEM_PROCESSOR}/$<IF:$<CONFIG:Debug>,Debug,Release>)
add_dependencies(${PROJECT_NAME} GenesisCore)

target_link_libraries(${PROJECT_NAME} PUBLIC
    GenesisCore
    GLEW::GLEW
    nlohmann_json::nlohmann_json
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    rpclib::rpc
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_GLOBAL_VcpkgEnableManifest true)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE FORGE_PROCESS_PORT=${FORGE_PROCESS_PORT})

target_compile_options(${PROJECT_NAME} PRIVATE
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