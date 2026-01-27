cmake_minimum_required(VERSION 3.17)

# set a variable in CMake
set(project_name "gr4-playground")
set(target_name "backend")

project(${project_name} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# dependencies: needs to be after adding the emscripten option so they are applied to dependency targets as well
#list(APPEND CMAKE_MODULE_PATH "./cmake/")
include(cmake/CMakeRC.cmake)
include(FetchContent)
include(cmake/CompileGr4Release.cmake)

# The blocklib_generator parser tool must be compiled for the host (not WASM).
# Override the compiler it uses since CMAKE_CXX_COMPILER is emcc here.
set(GNURADIO_PARSE_REGISTRATIONS_TOOL_CXX_COMPLILER /usr/bin/g++)

FetchContent_Declare(
  gnuradio4
  GIT_REPOSITORY https://github.com/fair-acc/gnuradio4.git
  GIT_TAG db5b4b92a3e886d070dd6454d7b2477c2cb1991f
  SYSTEM EXCLUDE_FROM_ALL)

FetchContent_MakeAvailable(gnuradio4)

od_set_release_flags_on_gnuradio_targets("${gnuradio4_SOURCE_DIR}")

add_executable(${target_name} backend.cpp)



#set(CMAKE_EXECUTABLE_SUFFIX ".html")
set(CMAKE_EXECUTABLE_SUFFIX ".js")
  # CAUTION: The "SHELL:" before some compile/link options has to be used to explicitly tell CMake to not sum up options
  # (e.g. -option A -option B => -option A B)(CMake default behaviour), but instead keep them separated (needed for
  # proper operation of emscripten)
set_target_properties(${target_name} PROPERTIES COMPILE_FLAGS "-g    -Wall     -Wformat     -Os    -fwasm-exceptions    -pthread")
set_target_properties(${target_name} PROPERTIES LINK_FLAGS
    "-s NO_EXIT_RUNTIME=1 -s \"EXPORTED_RUNTIME_METHODS=['ccall']\" -s WASM=1 -sFULL_ES2=1    -sMAX_WEBGL_VERSION=2    -sMIN_WEBGL_VERSION=2    -s ALLOW_MEMORY_GROWTH=1    -s ASSERTIONS=1    -s INITIAL_MEMORY=300MB    -s STACK_SIZE=5MB    -fwasm-exceptions    -pthread    -sFETCH=1    --bind    -Wl,--allow-multiple-definition"
  )

  #set_target_properties(${target_name} PROPERTIES COMPILE_FLAGS "-Os -s SIDE_MODULE=1 -Wno-error=old-style-cast -Wno-error=unused-parameter")
  #set_target_properties(${target_name} PROPERTIES LINK_FLAGS    "-Os -s WASM=1 -s SIDE_MODULE=1 -s STANDALONE_WASM --no-entry --disable-shared")

target_link_libraries(
  ${target_name}
  PUBLIC
  GrBasicBlocksShared
  GrTestingBlocksShared
  GrFileIoBlocksShared
  GrFilterBlocksShared
  embind
         )

