cmake_minimum_required(VERSION 3.16)
project(CPPMain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
find_package(CAF REQUIRED)

add_executable(main_program src/main.cpp src/main_actor.cpp src/worker_actor.cpp
                             src/result_collector.cpp src/sender.cpp src/receiver.cpp src/printer.cpp)

target_include_directories(main_program PRIVATE inc)
target_link_libraries(main_program OpenSSL::Crypto)
target_link_libraries(main_program CAF::core CAF::net)
