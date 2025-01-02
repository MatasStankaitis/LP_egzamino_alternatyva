cmake_minimum_required(VERSION 3.16)
project(CPPMain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
find_package(CAF REQUIRED)

file(GLOB SOURCES "src/*.cpp")
add_executable(main_program ${SOURCES})

target_include_directories(main_program PRIVATE
    inc
    configs
)
target_link_libraries(main_program CAF::core CAF::net)
