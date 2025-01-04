cmake_minimum_required(VERSION 3.16)
project(CPPMain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_CCACHE "Use ccache to speed up rebuilds" ON)
if(USE_CCACHE)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
  endif()
endif()

find_package(OpenSSL REQUIRED)
find_package(CAF REQUIRED)

file(GLOB SOURCES "src/*.cpp")
add_executable(main_program ${SOURCES})

target_include_directories(main_program PRIVATE
    inc
    configs
)
target_link_libraries(main_program CAF::core)
