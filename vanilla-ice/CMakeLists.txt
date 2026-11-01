cmake_minimum_required(VERSION 3.14)
project(vanilla-ice)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
)

include_directories(
    ${PROJECT_SOURCE_DIR}/inc
)
