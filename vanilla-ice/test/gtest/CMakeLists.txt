cmake_minimum_required(VERSION 3.14)
project(gtest)

set(CMAKE_CXX_STANDARD 23)

add_library(gtest
    STATIC 
    gmock-gtest-all.cc
    gmock/gmock.h gtest/gtest.h
    gtest/gtest-spi.h
)

target_compile_definitions(gtest PUBLIC GTEST_HAS_STD_WSTRING=1)
target_include_directories(gtest SYSTEM PUBLIC .)

find_package(Threads)

if (Threads_FOUND)
  target_link_libraries(gtest ${CMAKE_THREAD_LIBS_INIT})
else ()
  target_compile_definitions(gtest PUBLIC GTEST_HAS_PTHREAD=0)
endif ()

if (MSVC)
  target_compile_definitions(gtest PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()
