#-----------------------------------------------------------------#
#     _  _   __   __ _  __  __    __     __     __  ___  ____     #
#    / )( \ / _\ (  ( \(  )(  )  (  )   / _\   (  )/ __)(  _ \    #
#    \ \/ //    \/    / )( / (_/\/ (_/\/    \   )(( (__  )   /    #
#     \__/ \_/\_/\_)__)(__)\____/\____/\_/\_/  (__)\___)(__\_)    #
#-----------------------------------------------------------------#
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

set(
    GENA_SOURCES
    src/gena.cpp
    src/json-line.cpp
    src/sucker.cpp
)

set(
    ICE_SOURCES
    src/ice-runner.cpp
    src/ice.cpp
    src/json-line.cpp
    src/sha256.cpp
)

# Events from File Reader
# → Apply Rule (Sliding Window / Timestamps)
# → Generate Alert
add_executable(gena ${GENA_SOURCES})

# One-Shot Integrity Checker
add_executable(ice ${ICE_SOURCES})
target_link_libraries(ice PRIVATE OpenSSL::SSL)

# HTTP Client Sender
add_executable(sender src/sender.cpp)
target_link_libraries(sender PRIVATE OpenSSL::SSL)

# HTTP Server Collector
add_executable(colya src/colya.cpp)
target_link_libraries(colya PRIVATE OpenSSL::SSL)
