cmake_minimum_required(VERSION 3.17)

# set a variable in CMake
set(projectName "opendigitizer-ui")

project(${projectName} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

option(OD_DISABLE_DEMO_FLOWGRAPHS "Disable adding the demo flowgraphs to UI" OFF)
set(GR_MAX_WASM_THREAD_COUNT
    60
    CACHE STRING "Max number of threads for WASM pthread pool")


# dependencies: needs to be after adding the emscripten option so they are applied to dependency targets as well
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../../cmake")
include(../../cmake/CMakeRC.cmake)
include(cmake/Dependencies.cmake)

add_compile_definitions(GR_MAX_WASM_THREAD_COUNT=${GR_MAX_WASM_THREAD_COUNT}) # propagate to C++

if(NOT TARGET od_acquisition)
  add_subdirectory(../acquisition acquisition)
endif()
if(NOT TARGET raii_wrapper)
  add_subdirectory(../utils utils)
endif()


cmrc_add_resource_library(
  sample_dashboards
  NAMESPACE
  sample_dashboards
  assets/sampleDashboards/dashboard.grc
  assets/sampleDashboards/ExtendedDemoDashboard.grc
  assets/sampleDashboards/DemoDashboard.grc)

  set(target_name "opendigitizer-ui")
  add_executable(${target_name})


  if(OPENDIGITIZER_ENABLE_ASAN)
    target_compile_options(${target_name} PRIVATE -fsanitize=address)
    target_link_options(${target_name} PRIVATE -fsanitize=address)
  endif()


# All ui/ translation-units (except main.cpp) are built by ${target_name}lib static lib just so they can be shared by
# the tests and not build N times
add_library(
  ${target_name}lib STATIC main.hpp)

target_sources(${target_name} PRIVATE main.cpp)

target_link_libraries(
  ${target_name}lib
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

  target_link_libraries(${target_name}lib PUBLIC sample_dashboards)

target_compile_options(${target_name}lib PUBLIC "-Wfatal-errors")
target_link_libraries(${target_name} PRIVATE ${target_name}lib)

if(OPENDIGITIZER_ENABLE_TESTING)
  add_subdirectory(test)
endif()
