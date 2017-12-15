cmake_minimum_required(VERSION 3.2)
project(zerialize_tests)

set(${PROJECT_NAME}_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/gtest/gtest/gtest.h
        ${CMAKE_SOURCE_DIR}/gtest/gmock-gtest-all.cc
        ${CMAKE_SOURCE_DIR}/gtest/gmock/gmock.h
        AllTest.cpp
        Serialize.cpp
        )

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_SKIP_BUILD_RPATH false)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCE_FILES})

add_test(SerializeTest ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/gtest)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/gtest/gtest)
target_link_libraries(${PROJECT_NAME} zerialize  ${CMAKE_THREAD_LIBS_INIT})
