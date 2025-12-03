cmake_minimum_required(VERSION 3.17)

# set a variable in CMake
set(projectName "gr4-gui-backend")

project(${projectName} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# dependencies: needs to be after adding the emscripten option so they are applied to dependency targets as well
#list(APPEND CMAKE_MODULE_PATH "./cmake/")
include(cmake/CMakeRC.cmake)
include(FetchContent)
include(cmake/DependenciesSHAs.cmake)
include(cmake/CompileGr4Release.cmake)

FetchContent_Declare(
  gnuradio4
  GIT_REPOSITORY https://github.com/fair-acc/gnuradio4.git
  GIT_TAG ${GIT_SHA_GNURADIO4}
  SYSTEM EXCLUDE_FROM_ALL)

FetchContent_MakeAvailable(gnuradio4)

od_set_release_flags_on_gnuradio_targets("${gnuradio4_SOURCE_DIR}")

set(target_name "baseline")
add_executable(${target_name})


target_sources(${target_name} PRIVATE baseline.cpp)
target_compile_options(${target_name} PRIVATE -Wno-error=old-style-cast -Wno-error=useless-cast -Wno-error=unused-parameter)

target_link_libraries(
  ${target_name}
  PUBLIC
         gnuradio-core
         gnuradio-blocklib-core
         GrBasicBlocksShared
         GrElectricalBlocksShared
         GrFileIoBlocksShared
         GrFilterBlocksShared
         GrFourierBlocksShared
         GrHttpBlocksShared
         GrMathBlocksShared
         GrTestingBlocksShared)


set(target_name "backend")
add_executable(${target_name})


target_sources(${target_name} PRIVATE main.cpp)
target_compile_options(${target_name} PRIVATE -Wno-error=old-style-cast -Wno-error=useless-cast -Wno-error=unused-parameter)

target_link_libraries(
  ${target_name}
  PUBLIC
         gnuradio-core
         gnuradio-blocklib-core
         GrBasicBlocksShared
         GrElectricalBlocksShared
         GrFileIoBlocksShared
         GrFilterBlocksShared
         GrFourierBlocksShared
         GrHttpBlocksShared
         GrMathBlocksShared
         GrTestingBlocksShared)

