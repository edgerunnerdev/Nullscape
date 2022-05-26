cmake_minimum_required(VERSION 3.15)
project(Genesis)

add_subdirectory("libs/imgui")
add_subdirectory("libs/GenesisCore")
add_subdirectory("libs/GenesisResComp")
add_subdirectory("tools")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR}/src FILES ${SOURCE_FILES})

find_package(bullet3 REQUIRED)
find_package(CmdParser REQUIRED)
find_package(imgui REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(VPX REQUIRED)

include_directories(
  src/ 
  ${BULLET3_INCLUDE_DIRS}
  ${CMDPARSER_INCLUDE_DIRS}
  ${IMGUI_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${VPX_INCLUDE_DIRS}
)

add_library(Genesis STATIC ${SOURCE_FILES})
set_target_properties(Genesis PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/libs/Genesis/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_HOST_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE})

if(WIN32)
    find_package(FMOD REQUIRED)
    target_include_directories(Genesis PRIVATE ${FMOD_INCLUDE_DIRS})
    target_compile_definitions(Genesis PRIVATE UNICODE _UNICODE _HASEXCEPTIONS=0)
    target_compile_options(Genesis PUBLIC $<$<CONFIG:Final>:/Ob2 /GL>)
else()
    find_package(SDL2_mixer REQUIRED)
endif()

target_compile_definitions(Genesis PRIVATE $<$<CONFIG:Final>:NDEBUG _FINAL>)

target_compile_options(Genesis PRIVATE
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

target_compile_definitions(Genesis PRIVATE $<$<CONFIG:Debug>:OPENGL_ERROR_CHECKING=1> $<$<CONFIG:Release>:OPENGL_ERROR_CHECKING=1> $<$<CONFIG:Final>:OPENGL_ERROR_CHECKING=0>)