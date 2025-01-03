cmake_minimum_required(VERSION 3.0)

project(fgd2fgd LANGUAGES CXX)

set(SOURCE_FILES 
    src/main.cpp
    src/Fgd.cpp
    src/vectors.cpp
    src/util.cpp
)

include_directories(src)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-std=c++2a")
target_link_libraries(${PROJECT_NAME} fmt)
