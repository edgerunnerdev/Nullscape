cmake_minimum_required(VERSION 3.15)
set(PROJECT_NAME "Genesis")
project(${PROJECT_NAME})

include(ExternalLibraries)

set(FORGE_PROCESS_PORT 47563)
set(FORGE_LISTENER_PORT 47564)

add_subdirectory("libs/GenesisCore")
add_subdirectory("libs/GenesisResComp")
add_subdirectory("tools")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h src/*.hpp)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src FILES ${SOURCE_FILES})
list(APPEND SOURCE_FILES tools/modelcomp/src/modelserialization.hpp)

find_package(bitsery REQUIRED)
find_package(CmdParser REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(GenesisCore REQUIRED)
find_package(GLM REQUIRED)
find_package(MagicEnum REQUIRED)
find_package(rpclib REQUIRED)

include_directories(
    src/
    src/imgui/
    tools/modelcomp/src/
    ${BITSERY_INCLUDE_DIRS}
    ${CMDPARSER_INCLUDE_DIRS}
    ${GENESISCORE_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${MAGICENUM_INCLUDE_DIRS}
)

BuildExternalLibraries()

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/libs/Genesis/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_HOST_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC UNICODE _UNICODE _HASEXCEPTIONS=0)
    target_compile_options(${PROJECT_NAME} PUBLIC $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
    target_compile_options(${PROJECT_NAME} PUBLIC $<$<CONFIG:Release>:/Ob2 /GL>)
    target_compile_options(${PROJECT_NAME} PUBLIC /Zi) # Debug information format: Program database.
    target_compile_options(${PROJECT_NAME} PUBLIC /MP) # Enable parallel compilation.
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE FORGE_PROCESS_PORT=${FORGE_PROCESS_PORT} FORGE_LISTENER_PORT=${FORGE_LISTENER_PORT})

target_link_libraries(${PROJECT_NAME} PUBLIC 
    GLEW::GLEW
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    LinearMath Bullet3Common BulletDynamics BulletCollision
    rpclib::rpc
)

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

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:OPENGL_ERROR_CHECKING=1>)
add_dependencies(${PROJECT_NAME} bitsery MagicEnum xxhash)