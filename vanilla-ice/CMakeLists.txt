cmake_minimum_required(VERSION 3.14)
project(vanilla-ice)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenSSL REQUIRED)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wno-deprecated-declarations # for OpenSSL 3+ exclusively
)

include_directories(
    ${PROJECT_SOURCE_DIR}/inc
)

# HTTP Client Sender
add_executable(sender src/sender.cpp)
target_link_libraries(sender PRIVATE OpenSSL::SSL)

# HTTP Server Collector
add_executable(colya src/colya.cpp)
target_link_libraries(colya PRIVATE OpenSSL::SSL)
