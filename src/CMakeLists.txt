cmake_minimum_required(VERSION 3.2)
project(zerialize)

set(${PROJECT_NAME}_SOURCE_FILES
    Serialize.h
    Serialize.cpp
)

add_library(${PROJECT_NAME} OBJECT ${${PROJECT_NAME}_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC .)
#target_link_libraries(${PROJECT_NAME})
