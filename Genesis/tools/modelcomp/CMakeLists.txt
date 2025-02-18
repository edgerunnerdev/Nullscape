﻿cmake_minimum_required(VERSION 3.15)
set(PROJECT_NAME "ModelComp")
project(${PROJECT_NAME})

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.hpp)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src FILES ${SOURCE_FILES})

find_package(bitsery REQUIRED)
find_package(CmdParser REQUIRED)
find_package(GenesisCore REQUIRED)
find_package(GenesisResComp REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(
    src/
    ${BITSERY_INCLUDE_DIRS}
    ${CMDPARSER_INCLUDE_DIRS}
    ${GENESISCORE_INCLUDE_DIRS}
    ${GENESISRESCOMP_INCLUDE_DIRS}
)

set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../bin/compilers)

if(WIN32)
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE _HASEXCEPTIONS=0)
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Final>:/INCREMENTAL:NO /LTCG>)
else()
	add_executable(${PROJECT_NAME} ${SOURCE_FILES})
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()

target_link_directories(${PROJECT_NAME} PRIVATE 
    ${GENESISCORE_LIBRARY_DIRS}
    ${GENESISRESCOMP_LIBRARY_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    assimp::assimp
    CGAL::CGAL
    GenesisCore
    GenesisResComp
)

add_dependencies(${PROJECT_NAME} GenesisCore)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Final>:NDEBUG _FINAL>)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIRECTORY})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_FINAL ${OUTPUT_DIRECTORY})

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