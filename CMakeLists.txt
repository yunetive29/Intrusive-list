cmake_minimum_required(VERSION 3.14)
project(intrusive_list_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(doctest REQUIRED)

add_executable(test_run test.cpp)
target_link_libraries(test_run PRIVATE doctest::doctest)
target_include_directories(test_run PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
