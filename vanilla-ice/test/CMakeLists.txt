cmake_minimum_required(VERSION 3.14)
project(test)

set(CMAKE_CXX_STANDARD 23)
add_subdirectory(gtest)

enable_testing()

include_directories(
    ${PROJECT_SOURCE_DIR}/../inc
    ${PROJECT_SOURCE_DIR}
)

set(
    TEST_RUNNER_SRC
    smoke-test.cpp
    test-main.cpp
)

add_executable(test-runner ${TEST_RUNNER_SRC})
target_link_libraries(test-runner gtest)
