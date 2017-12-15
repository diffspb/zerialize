cmake_minimum_required(VERSION 2.8)

find_package(Threads)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)
enable_testing()
add_subdirectory(tests)

